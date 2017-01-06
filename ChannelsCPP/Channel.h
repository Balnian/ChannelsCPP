#pragma once
#include <queue>
#include <deque>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <limits>

#include "ChannelBuffer.h"
#include "IChannel.h"
#include "OChannel.h"
#include "ChannelUtility.h"
#include "Circular_buffer.h"

namespace go
{
	//Channel Type references: https://golang.org/ref/spec#Channel_types
	template<typename T, std::size_t Buffer_Size = 1>
	class Chan :public IChan<T, Buffer_Size>, public OChan<T, Buffer_Size>
	{
	public:
		Chan()
		{
			IChan::m_buffer = OChan::m_buffer = std::make_shared<internal::ChannelBuffer<T, Buffer_Size>>();
			//IChan<T, Buffer_Size>::m_buffer = OChan<T, Buffer_Size>::m_buffer = std::make_shared<internal::ChannelBuffer<T, Buffer_Size>>();
		}
		~Chan() = default;

		//Insert in channel
		friend 	OChan<T, Buffer_Size> operator<<(Chan<T, Buffer_Size>& ch, const T& obj)
		{
			return static_cast<OChan<T, Buffer_Size>>(ch) << obj;
			/*ch.m_buffer->insertValue(obj);
			return ch;*/
		}
		friend 	OChan<T, Buffer_Size> operator >> (const T& obj, Chan<T, Buffer_Size>& ch)
		{
			return static_cast<OChan<T, Buffer_Size>>(ch) << obj;

			/*ch.m_buffer->insertValue(obj);
			return  ch;*/

		}
		//Stream
		friend std::ostream& operator<<(std::ostream& os, Chan<T, Buffer_Size>& ch)
		{
			return os << static_cast<OChan<T, Buffer_Size>>(ch);
		}
		friend std::istream& operator >> (std::istream& is, Chan<T, Buffer_Size>& ch)
		{
			return is >> static_cast<IChan<T, Buffer_Size>>(ch);
		}



	};

	/*template<typename T>
	class Chan<T,0> :public IChan<T>, public OChan<T>
	{
	public:
		Chan()
		{
			IChan<T>::m_buffer = OChan<T>::m_buffer = std::make_shared<internal::ChannelBuffer<T>>();
		}
		~Chan() = default;

		//Insert in channel
		friend 	OChan<T> operator<<(Chan<T>& ch, const T& obj)
		{
			return static_cast<OChan<T>>(ch) << obj;
		}
		friend 	OChan<T> operator >> (const T& obj, Chan<T>& ch)
		{
			return static_cast<OChan<T>>(ch) << obj;

		}
		//Stream
		friend std::ostream& operator<<(std::ostream& os, Chan<T>& ch)
		{
			return os << static_cast<OChan<T>>(ch);
		}
		friend std::istream& operator >> (std::istream& is, Chan<T>& ch)
		{
			return is >> static_cast<IChan<T>>(ch);
		}



	};*/

}

//template<typename T>
//using inf_chan = Chan<T, std::numeric_limits<std::size_t>::infinity(), std::queue<T>>;

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

