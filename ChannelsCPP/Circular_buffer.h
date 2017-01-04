#pragma once
#include<iostream>
#include<memory>
#include <stdexcept>
namespace go
{
	namespace internal
	{
		template<typename T, std::size_t Buffer_Size>
		class Circular_buffer
		{
			typedef T& reference;
			typedef const T& const_reference;
			typedef T* pointer;
			typedef const T* const_pointer;
			typedef decltype(Buffer_Size) syze_type;

			std::unique_ptr<T[]> m_buffer;
			pointer m_front;
			pointer m_end;
			size_t m_size;

			void increment(pointer p)
			{
				if (p + 1 == &m_buffer[0] + Buffer_Size)
					p = &m_buffer[0];
				++p;

			}
		public:
			Circular_buffer()
				:m_buffer(new T[Buffer_Size]),
				m_front(&m_buffer[0]),
				m_end(&m_buffer[0]),
				m_size(0)
			{

			}

			~Circular_buffer()
			{
				m_front = nullptr;
				m_end = nullptr;

			}

			bool empty() const
			{
				return m_size <= 0;
			}

			bool full() const
			{
				return m_size == Buffer_Size;
			}

			const T& front() const
			{
				if (empty())
					throw std::out_of_range("container is empty");
				return *m_front;
			}

			const T& back() const
			{
				if (empty())
					throw std::out_of_range("container is empty");
				return *m_end
			}

			syze_type size() const
			{
				return m_size;
			}

			void push_back(T item)
			{
				if (full())
					throw std::out_of_range("container is full");
				*m_end = item;
				increment(m_end);
				++m_size;
			}

			template<typename ... Args>
			void emplace_back(Args && ... args)
			{
				push_back(T(std::forward<Args>(args)...));
			}

			T pop_front()
			{
				if (empty())
					throw std::out_of_range("container is empty");
				--m_size;
				T temp = *m_front;
				increment(m_front);
				return temp;
			}


		};
	}
}