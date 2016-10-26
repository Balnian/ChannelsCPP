#pragma once

#include <queue>
#include <memory>

#include <mutex>
#include <condition_variable>
#include <atomic>
namespace go
{
	namespace channel
	{
		template<typename T>
		class ChannelBuffer
		{
		private:
			std::queue<T> buffer;
			std::mutex bufferLock;
			std::condition_variable inputWait;
			std::atomic_bool is_closed;
		public:
			ChannelBuffer():is_closed(false) { }
			~ChannelBuffer() = default;

			//Wait for the next value
			T getNextValue()
			{
				std::unique_lock<std::mutex> ulock(bufferLock);
				if (buffer.empty())
				{
					if (is_closed) //if closed we always return the default initialisation of T
						return{};
					inputWait.wait(ulock, [&]() {return !buffer.empty() || is_closed; });
					if (buffer.empty() && is_closed) // when we close the channel and there was more waiting then available value 
						return{};

				}

				T temp;
				std::swap(temp, buffer.front());
				buffer.pop();
				return temp;
			}

			//Should use std::optional but MSVC and Clang doesn't support it yet :( #C++17
			std::unique_ptr<T> tryGetNextValue()
			{
				if (is_closed) //if closed we always return the default initialisation of T
					return std::make_unique<T>(T{});
				std::unique_lock<std::mutex> ulock(bufferLock);
				if (buffer.empty())
				{
					return nullptr;
				}
				std::unique_ptr<T> temp = std::make_unique<T>(buffer.front());
				buffer.pop();
				return std::move(temp);
			}

			void insertValue(T in)
			{
				if (!is_closed)
				{
					{
						std::lock_guard<std::mutex> lock(bufferLock);
						buffer.push(in);
					}
					inputWait.notify_one();
				}
			}

			void close()
			{
				is_closed = true;
				inputWait.notify_one();

			}

			bool isClosed()
			{
				return is_closed;
			}

		};
	}
}
