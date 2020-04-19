/*
 * Function.h
 *
 *  Created on: Apr 13, 2018
 *      Author: xupengtao
 */
#ifndef _CIRCULARQUEUE_H_
#define _CIRCULARQUEUE_H_

template <typename Type>
class CircularQueue
{
private:
	int Len_;
	int Begin;
	int Last;
	int Length_;
	Type* Array;
public:
	CircularQueue(int Len = 0x01);
	~CircularQueue();
	void WriteReset(int Ind);
	void ReadReset();
	void ReadContinue(int Ind);
	void CapacityReset(const int Capacity){Begin = 0;Len_ = Capacity;Last = Len_;Length_ = Len_+1;
										   delete[] Array; Array = new Type[Length_];
										   memset(Array,0,sizeof(Type)*Length_);}
	int  GetBufferCapacity() const {return Length_;}
	void IncArrayGen();
	void show(int num);
	Type & operator[](const int Ind);
};

template <typename Type>
CircularQueue<Type>::CircularQueue(int Len):Len_(Len)
{
	Begin = 0;
	Last = Len_;
	Length_ = Len_+1;
	Array = new Type[Length_];
	memset(Array,0,sizeof(Type)*Length_);
}

template <typename Type>
CircularQueue<Type>::~CircularQueue()
{
	delete[] Array;
}

template <typename Type>
void CircularQueue<Type>::WriteReset(int Ind)
{
	uint_t n;
	if(Ind+Begin <= Len_)
	{
		n = Ind+Begin;
	}
	else
	{
		n = Ind+Begin-Len_-1;
	}
	if(Last != Len_)
	{
		Begin = Last+1;
	}
	else
	{
		Begin = 0;
	}
	Last = n;
	if(Last > Begin)
	{
		Length_ = Last-Begin+1;
	}
	else
	{
		Length_ = Last+Len_-Begin+2;
	}
}

template <typename Type>
void CircularQueue<Type>::ReadReset()
{
	Length_ = Len_+1;
	if(Last == Len_)
	{
		Begin = 0;
	}
	else
	{
		Begin = Last+1;
	}
}


template <typename Type>
void CircularQueue<Type>::ReadContinue(int Ind)
{
	if(Ind+Begin <= Len_)
	{
		Last = Ind+Begin;
	}
	else if(Ind <= Len_)
	{
		Last = Ind+Begin-Len_-1;
	}
	else
	{
		Last =  Len_;
	}
	if(Last != Len_)
	{
		Begin = Last+1;
	}
	else
	{
		Begin = 0;
	}
}

template <typename Type>
void CircularQueue<Type>::IncArrayGen()
{
	for(int i=0;i<Len_;i++)
	{
		Array[i] = i;
	}
}

template <typename Type>
void CircularQueue<Type>::show(int num)
{
	NLogF("Array[%d] = %d",num,Array[num]);
}

template <typename Type>
Type & CircularQueue<Type>::operator[](const int Ind)
{
	if(Ind <= Length_)
	{
		if(Ind+Begin <= Len_)
		{
			return Array[Ind+Begin];
		}
		else
		{
			return Array[Ind+Begin-Len_-1];
		}
	}
	else
	{
		return Array[Last];
	}
}

#endif
