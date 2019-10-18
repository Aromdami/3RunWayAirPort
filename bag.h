#pragma once
#pragma once
#pragma once
#include <iostream>
#include <memory>
#include <cstdlib>
using namespace std;
#define MaxSize 100
template <class T>
class Bag {
public:
	Bag(int bagCapacity = 10);
	~Bag();
	bool IsFull();
	int Size() const;
	bool IsEmpty() const;
	virtual T& Pop() ;
	virtual void Push(const T&);
protected:
	T* b_array;
	int capacity;
	int top;
};
//Visual Studio2019에서 bag.h, bag.cpp로 분리하지 않아야 함
template <class T>
Bag<T>::Bag(int bagCapacity) : capacity(bagCapacity)
{
	if (capacity < 1) throw "Capacity must be > 0";
	b_array = new T[capacity];
	top = -1;
}

template <class T>
Bag<T>::~Bag() { delete[] b_array; }

template <class T>
void ChangeSizeID(T*& a, const int oldSize, const int newSize)
{
	if (newSize < 0) throw "New length must be >= 0";

	T* temp = new T[newSize];
	int number = oldSize;
	if (oldSize > newSize) number = newSize;
	//copy(a, a + number, temp);
	memcpy(temp, a, number);
	delete[] a;
	a = temp;
}

template <class T>
int Bag<T>::Size() const {
	return top + 1;
}

template <class T>
bool Bag<T>::IsEmpty() const {
	return top < 0;
}

template <class T>
void Bag<T>::Push(const T& x)
{
	if (top == capacity - 1)
		// 현재 버젼은 ordering 상태에서 push한다. non-ordering되게 push가 가능하게 수정
	{
		ChangeSizeID(b_array, capacity, 2 * capacity);
		capacity *= 2;
	}
	int position = rand() % capacity;
	while (b_array + position != NULL)
		position = rand() % capacity;
	b_array[position] = x;
	top++;
}

template <class T>
T& Bag<T>::Pop() 
{
	T retValue;
	if (IsEmpty()) throw "Bag is empty, cannot delete";
	int deletePos =0;
	while(b_array + deletePos != NULL)
		deletePos = rand() % capacity;
	retValue = b_array[deletePos];
	// 실습 사항: no ordering상태로 pop되게 수정
	//copy(b_array + deletePos + 1, b_array + top + 1, b_array + deletePos);
	memcpy(b_array + deletePos, b_array + deletePos + 1, sizeof(T) * (capacity - deletePos));
	top--;
	return retValue;
}

template <class T>
inline bool Bag<T>::IsFull()
{
	if (top == MaxSize - 1) return 1;
	else return 0;
}