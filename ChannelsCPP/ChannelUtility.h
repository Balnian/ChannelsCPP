#pragma once
//#include "Channel.h"
#include <utility>
#include <functional>
#include<iostream>
#include <random>
#include <algorithm>

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

		Case(const Case&) = default;
		Case() { task = []() {return true; }; }

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
		std::vector<Case> cases;

		bool randomExec()
		{
			std::random_device rd;
			std::mt19937 g(rd());
			std::shuffle(begin(cases), end(cases), g);
			for (auto& c : cases)
			{
				if (!c())
					return true;
			}
			return false;
		}

		template<typename ...T>
		void exec(Case && c, T &&... params)
		{
			cases.emplace_back(c);
			exec(std::forward<T>(params)...);
		}

		void exec(Case && c)
		{
			cases.emplace_back(c);
			randomExec();
		}

		void exec(Default && d)
		{
			if(!randomExec())
				d();
		}
		template<typename ...T>
		void exec(Default && c, T &&... params)
		{
			// Always false
			static_assert(static_cast<long long>(sizeof...(params)) == -1, "There should only be at most 1 Default case and it must be the last parameter of the Select");
		}

	public:
		template<typename ...T>
		Select(T &&... params):cases(sizeof...(params))
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

