#pragma once
//#include "Channel.h"
#include <utility>
#include <functional>

#include<iostream>
namespace chan
{
	/*template<typename T>
	class Optional :private std::pair<T, bool>
	{
	public:
		template<typename ...Args>
		Optional(Args &&... args) : std::pair<T, bool>(std::forward(args), false) {}
		bool operator

	};*/
	template<typename T> class Channel;
	/*class NoOpSelect {};
	class OpSelect{};
	template<typename TT=OpSelect>
	class Select
	{

	public:
		template<typename T, typename func>
		static Select& Case(Channel<T> chan, func f)
		{
			auto val = chan.m_channel->tryGetNextValue();
			if (val)
			{
				f(*val);
				return Select<NoOpSelect>;
			}
			else
				return this;
		}
		template<typename func>
		static void Default(func f)
		{
			f();
		}
	};
	template<>
	class Select<NoOpSelect>
	{
		template<typename T, typename func>
		static Select& Case(Channel<T> chan, func f) {}
		template<typename func>
		static void Default(func f) {}
	};*/
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

