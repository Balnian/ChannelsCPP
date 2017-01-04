#pragma once
//#include "Channel.h"
#include <utility>
#include <functional>
#include<iostream>

/*#include "IChannel.h"
#include "OChannel.h"*/

namespace go
{
	template<typename T> class Chan;

	template<typename T> class OChan;
	template<typename T> class IChan;


	// Select statements references: https://golang.org/ref/spec#Select_statements
	class Case
	{
		std::function<bool()> task;
	public:
		template<typename T, typename func>
		Case(IChan<T> ch, func f)
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

		template<typename T, typename func>
		Case(OChan<T> ch, func f)
		{
			task = [=]() {
				f();
				return true;
			};
		}

		template<typename T, typename func>
		Case(Chan<T> ch, func f) : Case(IChan<T>(ch), std::forward<func>(f)) {}


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
	template<typename T>
	void Close(OChan<T> ch)
	{
		ch.close();
	}

	template<typename T>
	Chan<T> && make_Chan()
	{
		return Chan<T>();
	}


}

