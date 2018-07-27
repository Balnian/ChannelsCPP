# ChannelsCPP
C++ implementation of the Go (GoLang) Channel and Select structure in a modern C++ manner. 
  
You will need a C++ compiler that has at least support for C++11 and part of C++14 (it might work on C++11 only but i didn't test).  
I have already identified parts of the code that could use C++17 features, but some feature are not yet implemented in my compiler.  
  
I also want to stabilize the code a bit more, do performance testing/ C++ version testing and organise the repo (when i do C++17 integration make a branch for each C++ version that i can support down to C++11 ... if it has all the features ... still not tested)  
  
In the future i will try to bring each implementation closer to the Go implementation by respecting the API design and behavior.
## Features list
- [x] [Basic Go Channel type](#basic-channel) (will be modify when the [Buffered Channel](#buffered-channel) is implemented)
- [x] [Buffered Go Channel type](#buffered-channel)
- [x] Channel stream input/output operator overload for inserting and extracting
- [x] [Select statement](#select)
- [x] [Select statement](#select) with channel insertion
- [x] [Select statement](#select) random case selection when multiple available
- [ ] Custom For class, for cleaner use with select (no bool value)
- [x] Range for loop on channel
- [x] Close function (ties in with the Range for loop feature)
- [ ] make_Chan function to replace the [make](https://golang.org/ref/spec#Making_slices_maps_and_channels) function in Go

## Features
### Channels
Channel are simple and intuitive to use and interact like the [Go channel type](https://tour.golang.org/concurrency/2)  
Ther are two types of channel in go, the [basic channel type](https://tour.golang.org/concurrency/2) and the [buffered channel type](https://tour.golang.org/concurrency/3)  
> Channels are a typed conduit through which you can send and receive values.  
>-Go  
  
* Multi directional inserting operators overload (The data flows in the direction of the insertion/extraction operator.)
* Multi directional extration operators overload (The data flows in the direction of the insertion/extraction operator.)
* Basic stream operator overload
* Channel to Channel inserting and extration
* Chaining insertion and extraction
* Iterators to work with C++ Range for loop (emulate the Go feature)
  
#### Unbuffered Channel
Unbuffered Channel only have 1 slot for data, so it's not exactly Unbuffered and as such does not behave like the Unbuffered Channel in Go that block until there's a sender and a receiver ready at the same time.  (It will change in the future)

```C++
Chan<int> ch;
// inserting 
int i = 2;
ch << 1;
i >> ch;
// extracting
i << ch;
ch >> y
```
#### Buffered Channel
Buffered Channel are a type of channel that doesn't block unless the buffer is full.

```C++
Chan<int, 5> multi;

thread([&]() {
	multi << 1 << 2 << 3 << 4 << 5;
	this_thread::sleep_for(chrono::milliseconds(500)); // Give some time to process the data
	Close(multi);
}).detach();

for(auto& asd:multi)
{
	cout << asd << endl;
}
```

### Select
The C++ implementation of the Go Select statement  

* can have one or infinite [Case](#case) block (infinite = until you connot compile it)
* can have zero or one [Default](#default) block (enforced in code)
* support input in channel in [Case](#case) block channel evaluation (feature seems to be working but might need a bit more attention)
* random channel ([Case](#case)) selection is now supported
  
Go exemple (from [here](https://tour.golang.org/concurrency/5)) :  
```Go
func fibonacci(c, quit chan int) {
	x, y := 0, 1
	for {
		select {
		case c <- x:
			x, y = y, x+y
		case <-quit:
			fmt.Println("quit")
			return
		}
	}
}

func main() {
	c := make(chan int)
	quit := make(chan int)
	go func() {
		for i := 0; i < 10; i++ {
			fmt.Println(<-c)
		}
		quit <- 0
	}()
	fibonacci(c, quit)
}

```

C++ exemple implementation :
```C++ 
void fibonacci(Chan<int>& c, Chan<int>& quit)
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

int main()
{
	Chan<int> c;
	Chan<int> quit;

	thread([&]()
	{
		for (size_t i = 0; i < 10; i++)
		{
			cout << c << endl;
		}
		quit << 0;
	}).detach();
	fibonacci(c, quit);
}
```

#### Case
Contains code to be executed when the channel that it's checking receive something.  
It support sending to the channel it's checking but the feature might need to be rework because it doesn't behave like the Go implementation.

#### Default
Execute it's code when no [Case](#case) block  had a channel that received data.  
There can only be at most **1** Default block in a [Select](#select) statement and it must be the last block (this is enforced in the code).

### Close
Function called on a [Channel](#channels) to close the stream.  
Most of the time it's use to notify a [Channel](#channels) that is being iterated over in a Range for loop. It is in fact the only way to exit a Range for loop.

### make_Chan
Is included, but doesn't work! **DO NOT USE**
