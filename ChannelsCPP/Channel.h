#pragma once
#include <queue>
#include <deque>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "ChannelBuffer.h"
#include "IChannel.h"
#include "OChannel.h"
#include "ChannelUtility.h"

namespace go
{
	//Channel Type references: https://golang.org/ref/spec#Channel_types
	template<typename T>
	class Chan :public channel::IChan<T>, public channel::OChan<T>
	{
	public:
		Chan()
		{
			channel::IChan<T>::m_buffer = channel::OChan<T>::m_buffer = std::make_shared<channel::ChannelBuffer<T>>();
		}
		~Chan() = default;

		//Insert in channel
		friend 	channel::OChan<T>& operator<<(Chan<T>& ch, const T& obj)
		{
			return static_cast<channel::OChan<T>>(ch) << obj;
			/*ch.m_buffer->insertValue(obj);
			return ch;*/
		}
		friend 	channel::OChan<T>& operator >> (const T& obj, Chan<T>& ch)
		{
			return static_cast<channel::OChan<T>>(ch) << obj;

			/*ch.m_buffer->insertValue(obj);
			return  ch;*/

		}
		//Stream
		friend std::ostream& operator<<(std::ostream& os, Chan<T>& ch)
		{
			return os << static_cast<channel::OChan<T>>(ch);
		}
		friend std::istream& operator >> (std::istream& is, Chan<T>& ch)
		{
			return is >> static_cast<channel::IChan<T>>(ch);
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