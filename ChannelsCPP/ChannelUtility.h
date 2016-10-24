#pragma once
//#include "Channel.h"
#include <utility>
#include <functional>

#include<iostream>
namespace chan
{
	template<typename T> class Channel;
	template<typename T> class OutChannel;
	
	class Case
	{
		std::function<bool()> task;
	public:
		template<typename T, typename func>
		Case(Channel<T> ch, func f)
		{
			task = [=]() {
				auto val = ch.m_channel->tryGetNextValue();
				if (val)
				{
					f(*val);
				};
				return val == nullptr;
			};
		}

		template<typename T, typename func>
		Case(OutChannel<T> ch, func f)
		{
			task = [=]() {
				f();
				return true;
			};
		}

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

	public:
		template<typename ...T>
		Select(T &&... params)
		{
			exec(std::forward<T>(params)...);
		}
	};


}

