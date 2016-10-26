#pragma once
#include <queue>
#include <deque>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>


#include "ChannelUtility.h"

namespace go
{
	template<typename T>
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

	template<typename T>
	class OChan //: private Chan<T>
	{
	protected:
		std::shared_ptr<ChannelBuffer<T>> m_buffer;
		OChan(std::shared_ptr<ChannelBuffer<T>> buffer) : m_buffer(buffer) {}
	public:
		OChan() = default;
		OChan(const OChan<T>& ch) = default;// :m_buffer(ch.m_buffer) {}
		//Insert in channel
		friend 	OChan<T>& operator<<(OChan<T>& ch, const T& obj)
		{
			ch.m_buffer->insertValue(obj);
			return ch;
		}
		friend 	OChan<T>& operator >> (const T& obj, OChan<T>& ch)
		{
			ch.m_buffer->insertValue(obj);
			return  ch;

		}

		//Stream
		friend std::ostream& operator<<(std::ostream& os, OChan<T>& ch)
		{
			os << ch.m_buffer->getNextValue();
			return os;
		}
	};

	template<typename T>
	class IChan //: private Chan<T>
	{
	protected:
		std::shared_ptr<ChannelBuffer<T>> m_buffer;
		IChan(std::shared_ptr<ChannelBuffer<T>> buffer) : m_buffer(buffer) {}
	public:
		IChan() = default;
		IChan(const IChan<T>& ch) = default;//:m_buffer(ch.m_buffer) {}
		//Extract from channel
		friend 	IChan<T>& operator >> (IChan<T>& ch, T& obj)
		{
			obj = ch.getNextValue();
			return ch;

		}
		friend 	IChan<T>& operator<<(T& obj, IChan<T>& ch)
		{
			obj = ch.getNextValue();
			return ch;
		}

		//Stream
		friend std::istream& operator >> (std::istream& is, IChan<T>& ch)
		{
			T temp;
			is >> temp;
			ch << temp;
			return is;
		}

		friend class Case;

	};

	template<typename T>
	class Chan:public  IChan<T>, public OChan<T>
	{
	public:
		Chan()
		{
			IChan<T>::m_buffer = OChan<T>::m_buffer = std::make_shared<ChannelBuffer<T>>();
		}
		~Chan()=default;

		
		//Stream
		friend std::ostream& operator<<(std::ostream& os, Chan<T>& ch)
		{
			return os << static_cast<OChan<T>>(ch);
		}
		friend std::istream& operator >> (std::istream& is, Chan<T>& ch)
		{
			return is >> static_cast<IChan<T>>(ch);
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