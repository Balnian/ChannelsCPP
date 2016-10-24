#pragma once
#include <queue>
#include <deque>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>


#include "ChannelUtility.h"

namespace chan
{

	template<typename T>
	class Channel
	{
	private:
		class ChannelBuffer
		{
		private:
			std::queue<T> buffer;
			std::mutex bufferLock;
			std::condition_variable inputWait;
		public:
			ChannelBuffer() = default;
			~ChannelBuffer() = default;
			T getNextValue()
			{
				std::unique_lock<std::mutex> ulock(bufferLock);
				if (buffer.empty())
				{
					inputWait.wait(ulock, [&]() {return !buffer.empty(); });
				}
				T temp;
				std::swap(temp, buffer.front());
				buffer.pop();
				return temp;
			}

			//Should use std::optional but MSVC and Clang doesn't support it yet :( #C++17
			std::unique_ptr<T> tryGetNextValue()
			{
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
				{
					std::lock_guard<std::mutex> lock(bufferLock);
					buffer.push(in);
				}
				inputWait.notify_one();
			}

		};
		std::shared_ptr<ChannelBuffer> m_channel ;
		T getNextValue()
		{
			return m_channel->getNextValue();
		}

		void insertValue(T val)
		{
			m_channel->insertValue(val);
		}

	public:
		Channel()
		{
			m_channel = std::make_shared<ChannelBuffer>();
		}
		~Channel()=default;

		//Extract from channel
		friend 	T& operator >> (Channel<T>& ch, T& obj)
		{
			obj = ch.getNextValue();
			return obj;

		}
		friend 	T& operator<<(T& obj, Channel<T>& ch)
		{
			obj = ch.getNextValue();
			return obj;
		}

		//Insert in channel
		friend 	OutChannel<T>& operator<<(Channel<T>& ch, const T& obj)
		{
			ch.insertValue(obj);
			return OutChannel<T>(ch);
		}
		friend 	OutChannel<T>& operator >> (const T& obj, Channel<T>& ch)
		{
			ch.insertValue(obj);
			return  OutChannel<T>(ch);

		}

		//Stream
		friend std::ostream& operator<<(std::ostream& os, Channel<T>& ch)
		{
			os << ch.getNextValue();
			return os;
		}
		friend std::istream& operator >> (std::istream& is, Channel<T>& ch)
		{
			T temp;
			is >> temp;
			ch << temp;
			return is;
		}

		friend class Case;
		

	};

	template<typename T>
	class OutChannel : private Channel<T>
	{
	public:
		OutChannel(Channel<T> ch) :Channel<T>(ch) {}
		//Insert in channel
		friend 	OutChannel<T>& operator<<(OutChannel<T>& ch, const T& obj)
		{
			ch.insertValue(obj);
			return *this;
		}
		friend 	OutChannel<T>& operator >> (const T& obj, OutChannel<T>& ch)
		{
			ch.insertValue(obj);
			return  *this;

		}
	};

	

	//Specialized (Buffered)
	/*template<typename T, std::size_t bufferSize>
	class Channel
	{
	private:
		class ChannelBuffer
		{
		private:
			std::deque<T> buffer;
			std::mutex bufferLock;
			std::condition_variable inputWait;
		public:
			ChannelBuffer() = default;
			~ChannelBuffer() = default;
			T getNextValue()
			{
				std::unique_lock<std::mutex> ulock(bufferLock);
				if (buffer.empty())
				{
					inputWait.wait(ulock, [&]() {return !buffer.empty(); });
				}
				T temp;
				std::swap(temp, buffer.front());
				buffer.pop_front();
				return temp;
			}
			void insertValue(T in)
			{
				{
					std::lock_guard<std::mutex> lock(bufferLock);
					if (buffer.size() >= bufferSize)
						throw exception("Buffer full");
					buffer.push_back(in);
				}
				inputWait.notify_one();
			}

		};
		std::shared_ptr<ChannelBuffer> m_channel;
		T getNextValue()
		{
			return m_channel->getNextValue();
		}

		void insertValue(T val)
		{
			m_channel->insertValue(val);
		}

	public:
		Channel()
		{
			m_channel = std::make_shared<ChannelBuffer>();
		}
		~Channel() = default;

		//Extract from channel
		friend 	T& operator >> (Channel<T, bufferSize>& ch, T& obj)
		{
			obj = ch.getNextValue();
			return obj;

		}
		friend 	T& operator<<(T& obj, Channel<T, bufferSize>& ch)
		{
			obj = ch.getNextValue();
			return obj;
		}

		//Insert in channel
		friend 	Channel<T, bufferSize>& operator<<(Channel<T, bufferSize>& ch, const T& obj)
		{
			ch.insertValue(obj);
			return ch;
		}
		friend 	Channel<T, bufferSize>& operator >> (const T& obj, Channel<T, bufferSize>& ch)
		{
			ch.insertValue(obj);
			return ch;

		}

		//Stream
		friend std::ostream& operator<<(std::ostream& os, Channel<T, bufferSize>& ch)
		{
			os << ch.getNextValue();
			return os;
		}
		friend std::istream& operator >> (std::istream& is, Channel<T, bufferSize>& ch)
		{
			T temp;
			is >> temp;
			ch << temp;
			return is;
		}


	};*/

}