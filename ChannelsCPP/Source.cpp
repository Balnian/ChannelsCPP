#include <iostream>
#include "Channel.h"
#include <thread>
#include <chrono>
#include <string>

using namespace std;
using namespace chan;

template<typename T>
void f(Channel<T>& ch, T x, int time)
{
	this_thread::sleep_for(chrono::seconds(time));
	ch << x;
}

int main()
{
	Channel<int> ch;
	Channel<string> ch2;

	auto t1 = thread(f<int>, ref(ch), 1, 3);
	auto t2 = thread(f<string>, ref(ch2), "banane",1 );

	for (bool asd = true; asd;)
	{
		Select
		{
			Case{ch,[&](int x)
			{
				cout << x << endl << flush;
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