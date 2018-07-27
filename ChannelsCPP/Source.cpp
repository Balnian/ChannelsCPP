#include <iostream>
#include "Channel.h"
#include <thread>
#include <chrono>
#include <string>
#include <vector>

#include "Circular_buffer.h"

using namespace std;
using namespace go;
using namespace go::internal;

template<size_t BufferSize>
void fibonacci(Chan<int, BufferSize>& c, Chan<int>& quit)
{
	int x=0, y = 1;
	for (bool go = true; go;)
	{
		Select
		{
			Case{c << x,[&]()
			{
				int t = x;
				x = y;
				y += t;
			}},
			Case{quit,[&](auto v) 
			{
				cout << "quit" << endl;
				go = false;
			}}
		};
	}
}
template<typename T1, typename T2>
void f(T1& ch, T2 x, int time)
{
	this_thread::sleep_for(chrono::seconds(time));
	ch << x;
}
int main()
{
	/*Circular_buffer<int,2> buff;
	// Try on empty
	try
	{
		cout << buff.pop_front() << endl;
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
	//testz
	try
	{
		buff.emplace_back(5);
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
	// Test Pop
	try
	{
		cout << buff.pop_front() << endl;
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
	//insert
	try
	{
		buff.emplace_back(5);
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
	//full
	try
	{
		buff.emplace_back(5);
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
	//exception full
	try
	{
		buff.emplace_back(5);
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}*/


	{
		Chan<int> c;
		thread([=]() mutable
		{
			for (size_t i = 0; i < 10; i++)
			{
				cout << c << endl;
			}
		}).detach();

		for (size_t i = 0; i < 10; i++)
		{
			c << i;
		}
		this_thread::sleep_for(chrono::milliseconds(1)); // wait for last entry to be printed
	}

	cout << "------Demo fibonacci (https://tour.golang.org/concurrency/5)-----" << endl;

	Chan<int> c;
	Chan<int> quit;

	c << 78;
	cout << c << endl;

	thread([&]()
	{
		for (size_t i = 0; i < 10; i++)
		{
			cout << c << endl;
		}
		quit << 0;
	}).detach();
	fibonacci(c, quit);

	cout << "------Demo with Default-----" << endl;

	Chan<int> ch;
	Chan<string> ch2;

	auto t1 = thread(f<decltype(ch),int>, ref(ch), 1, 3);
	auto t2 = thread(f<decltype(ch2),string>, ref(ch2), "J'aime",1 );

	for (bool asd = true; asd;)
	{
		Select
		{
			Case{ch,[&](int x)
			{
				cout << "mon prof !!!" << endl << flush;
				cout << "Valeur recu: " << x << endl << flush;
				asd = false;
			}},
			Case{ch2,[](string x)
			{
				cout << x << endl << flush;
			}},
			Default{[]()
			{
				cout << "Waiting...." << endl<<flush;
				this_thread::sleep_for(chrono::milliseconds(500));
			}}
		};
	}

	t1.join();
	t2.join();


	cout << "------Demo range for on channel-----" << endl;

	Chan<int> ch3;

	thread t3 = thread([&]() 
	{
		for (int i = 0; i < 10; i++)
		{
			ch3 << i;
			this_thread::sleep_for(chrono::seconds(1));
		}
		Close(ch3);
	});

	for (auto &asd : ch3)
	{
		cout << asd << endl;
	}
	cout << "Fini" << endl;
	t3.join();
	
	//must be fix
	cout << "------Test closing Channel-----" << endl;
	Chan<int> tested;
	Chan<int> output;
	vector<thread> vt;
	for (size_t i = 0; i < 10; i++)
	{
		vt.emplace_back([&]() 
		{
			output << tested;
		});
	}
	this_thread::sleep_for(chrono::seconds(1));
	tested << 3 << 2 << 1; //doesn't work... find why (atomic not initialized)
	1 >> (2 >> (3 >> (4 >> tested)));
	//Close(tested);
	int i = 0;
	for (auto &asd : output)
	{
		++i;
		cout << output << endl;
	}
	cout << "i: " << i << endl;
	for (auto &t : vt)
	{
		t.join();
	}
}