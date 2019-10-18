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
//Visual Studio2019���� bag.h, bag.cpp�� �и����� �ʾƾ� ��
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
		// ���� ������ ordering ���¿��� push�Ѵ�. non-ordering�ǰ� push�� �����ϰ� ����
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
	// �ǽ� ����: no ordering���·� pop�ǰ� ����
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