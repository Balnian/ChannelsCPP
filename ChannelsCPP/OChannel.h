#pragma once

#include "ChannelBuffer.h"

namespace go
{
	namespace channel
	{
		template<typename T> class IChan;

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

			friend 	OChan<T>& operator<<(OChan<T>& ch, const IChan<T>& obj)
			{
				T temp;
				temp << obj;
				ch << temp;
				return ch;
			}
			friend 	OChan<T>& operator >> (const IChan<T>& obj, OChan<T>& ch)
			{
				T temp;
				temp << obj;
				ch << temp;
				return  ch;

			}

			//Stream
			friend std::ostream& operator<<(std::ostream& os, OChan<T>& ch)
			{
				os << ch.m_buffer->getNextValue();
				return os;
			}

			void close()
			{
				m_buffer->close();
			}
		};
	}
}