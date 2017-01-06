#pragma once
//#include "Channel.h"
#include <utility>
#include <functional>
#include<iostream>

/*#include "IChannel.h"
#include "OChannel.h"*/

namespace go
{
	template<typename T, std::size_t Buffer_Size> class Chan;

	template<typename T, std::size_t Buffer_Size> class OChan;
	template<typename T, std::size_t Buffer_Size> class IChan;


	// Select statements references: https://golang.org/ref/spec#Select_statements
	class Case
	{
		std::function<bool()> task;
	public:
		template<typename T, std::size_t Buffer_Size, typename func>
		Case(IChan<T, Buffer_Size> ch, func f)
		{
			task = [=]() {
				auto val = ch.m_buffer->tryGetNextValue();
				if (val)
				{
					f(*val);
				};
				return val == nullptr;
			};
		}

		template<typename T, std::size_t Buffer_Size, typename func>
		Case(OChan<T, Buffer_Size> ch, func f)
		{
			task = [=]() {
				f();
				return true;
			};
		}

		template<typename T, std::size_t Buffer_Size, typename func>
		Case(Chan<T, Buffer_Size> ch, func f) : Case(IChan<T, Buffer_Size>(ch), std::forward<func>(f)) {}


		bool operator() ()
		{
			return task();
		}
	};

	class Default
	{
		std::function<void()> task;
	public:
		template<typename func>
		Default(func f)
		{
			task = f;
		}

		void operator() ()
		{
			task();
		}
	};

	class Select
	{
		template<typename ...T>
		void exec(Case && c, T &&... params)
		{
			if (c())
				exec(std::forward<T>(params)...);
		}

		void exec(Case && c)
		{
			c();
		}

		void exec(Default && d)
		{
			d();
		}
		template<typename ...T>
		void exec(Default && c, T &&... params)
		{
			static_assert(false, "There should only be at most 1 Default case and it must be the last parameter of the Select");
		}

	public:
		template<typename ...T>
		Select(T &&... params)
		{
			exec(std::forward<T>(params)...);
		}
	};

	// Close references: https://golang.org/ref/spec#Close
	// we try to avoid exceptions so we will have custom implementation
	template<typename T, std::size_t Buffer_Size>
	void Close(OChan<T, Buffer_Size> ch)
	{
		ch.close();
	}

	template<typename T, std::size_t Buffer_Size>
	Chan<T, Buffer_Size> && make_Chan()
	{
		return Chan<T, Buffer_Size>();
	}


}

