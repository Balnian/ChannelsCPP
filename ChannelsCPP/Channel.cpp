#include "Channel.h"
using namespace go;


/*template<typename T, unsigned int buffSize>
T chan::Channel<T, buffSize>::getNextValue()
{
	return T();
}

template<typename T, unsigned int buffSize>
chan::Channel<T, buffSize>::Channel()
{
}

template<typename T, unsigned int buffSize>
chan::Channel<T, buffSize>::~Channel()
{
}
*/

template<typename T>
T& operator<<(Chan<T>& ch, const T& obj)
{
	ch.insertValue(obj);
	return ch;
}


template<typename T>
Chan<T>& operator >> (Chan<T>& ch, T& obj)
{
	obj = ch.getNextValue();
	return obj;

}