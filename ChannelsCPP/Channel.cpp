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

template<typename T, std::size_t Buffer_Size>
T& operator<<(Chan<T, Buffer_Size>& ch, const T& obj)
{
	ch.insertValue(obj);
	return ch;
}


template<typename T, std::size_t Buffer_Size>
Chan<T, Buffer_Size>& operator >> (Chan<T, Buffer_Size>& ch, T& obj)
{
	obj = ch.getNextValue();
	return obj;

}