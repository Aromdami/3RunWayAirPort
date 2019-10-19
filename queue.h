#pragma once
#include "bag.h"
using namespace std;
#include <iomanip>
template <class T>
class Queue : public Bag<T>
{
public:
	Queue(int queueCapacity = 10);
	T& Front() const;
	T& Rear() const;
	T& Find(int ind) const;
	void Push(const T& item);
	T& Pop();
	T& searchPop(const T& item);
};

template <class T>
Queue<T>::Queue(int queueCapacity)
{
	this->b_array = new T[queueCapacity];
}
template  <class T>
T& Queue<T>::Front() const
{
	return this->b_array[0];
}

template <class T>
T& Queue<T>::Rear() const
{
	return this->b_array[this->top];
}

template <class T>
T& Queue<T>::Find(int ind) const
{
	return this->b_array[ind];
}

template <class T>
void Queue<T>::Push(const T& item)
{
	if (this->top == this->capacity - 1)
	{
		ChangeSizeID(this->b_array, this->capacity, 2 * this->capacity);
		this->capacity *= 2;
	}
	else
		this->b_array[++(this->top)] = item;

}


template <class T>
T& Queue<T>::Pop()
{
	T retValue;

	if (this->IsEmpty()) throw "Queue is Empty. Can not delete more items.";
	else
	{
		retValue = this->b_array[0];
		memcpy(this->b_array, this->b_array + 1, sizeof(T) * (this->capacity - 1));
		this->top--;
		return retValue;
	}

}

template <class T>
T& Queue<T>::searchPop(const T& item)
{
	T retValue;
	int i;
	for (i = 0; i < this->capacity; i++)
	{
		if (this->b_array[i] == item)
		{
			retValue = this->b_array[i];
			break;
		}
	}

	memcpy(this->b_array + i, this->b_array + i + 1, sizeof(T) * (this->capacity - i));

	this->top--;
	return retValue;
}