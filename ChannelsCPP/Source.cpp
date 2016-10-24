#include <iostream>
#include "Channel.h"
#include <thread>
#include <chrono>
#include <string>

using namespace std;
using namespace chan;


void fibonacci(Channel<int>& c, Channel<int>& quit)
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
template<typename T>
void f(Channel<T>& ch, T x, int time)
{
	this_thread::sleep_for(chrono::seconds(time));
	ch << x;
}
int main()
{
	cout << "------Demo fibonacci (https://tour.golang.org/concurrency/5)-----" << endl;

	Channel<int> c;
	Channel<int> quit;

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

	Channel<int> ch;
	Channel<string> ch2;

	auto t1 = thread(f<int>, ref(ch), 1, 3);
	auto t2 = thread(f<string>, ref(ch2), "J'aime",1 );

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


	

}