/*
 * Function.h
 *
 *  Created on: Apr 13, 2018
 *      Author: xupengtao
 */
#ifndef _CIRCULARLIST_H_
#define _CIRCULARLIST_H_

#include"../Macro.h"

enum CLWriteAndReadStatus {WriteEmpty, Writing, Reading};
enum CLLinkStatus {Opening, Linked};

template <typename T>
struct ListNode
{
	T data;
	ListNode<T>* next;
	int Index;
};

template <typename Type>
class CircularList
{
private:
	ListNode<Type>* Array;
	ListNode<Type>* pWriteHead_;
	ListNode<Type>* pWriteRear_;
	ListNode<Type>* pReadHead_;
	ListNode<Type>* pReadRear_;
	ListNode<Type>* pWrite;
	ListNode<Type>* pRead;
	ListNode<Type>* pNextChain;
	
	int WriteInd;
	int ReadInd;
	int Length_;												//链表数组Array可写入的的总长度，为最大索引值＋1
	static int CircularListNum;
	
public:					
	CLWriteAndReadStatus WRStatus;
	CLLinkStatus		 LinkStatus;
	CircularList<Type>*	 pNextCircularChain;
	
	CircularList(int Length = 0x10);
	~CircularList();
	void 	CapacityReset(const int Length);
	
	int		GetLength() const {return Length_;}
	int 	GetBufferCapacity();
	void	WriteHeadRearSet(ListNode<Type>* HeadSet,ListNode<Type>* RearSet){pWriteHead_ = HeadSet;pWriteRear_ = RearSet;}
	void	ReadHeadRearSet(ListNode<Type>* HeadSet,ListNode<Type>* RearSet){pReadHead_ = HeadSet;pReadRear_ = RearSet;}
	
	ListNode<Type>* GetArrayHeadPoint() const {return &Array[0];}
	ListNode<Type>* GetArrayIndPoint(const unsigned int Ind) const {return &Array[Ind];}
	int 	ChainLink(CircularList<Type>& NextChain);
	int 	SetNextChain(CircularList<Type>& NextChain);
	void	SetNextFriendChain(CircularList<Type>& NextChain){pNextCircularChain = &NextChain;}
	int		ChainLinkCheck();
	
	void 	WriteReset();							
	void 	WriteReset(int Ind);							
	void 	ReadReset();
	Type & 	operator[](const int Ind);
	
	void 	show(const int Ind = 0);
	void 	show(const int BeginInd,const int EndInd);
};

template <typename Type>
int CircularList<Type>::CircularListNum = 0;

template <typename Type>
CircularList<Type>::CircularList(int Length):Length_(Length+1)
{
	WRStatus = WriteEmpty;
	LinkStatus = Opening;
	Array = new ListNode<Type>[Length_];
	memset(Array,0,sizeof(ListNode<Type>)*Length_);
	for(int i=0;i<Length;i++)
	{
		Array[i].Index = i;
		Array[i].next = &Array[i+1];
	}
	Array[Length].Index = Length;
	Array[Length].next = NULL;
	pNextChain = NULL;
	pNextCircularChain = NULL;
	pWriteHead_ = &Array[0];
	pWriteRear_ = &Array[Length];
	pReadHead_ = &Array[0];
	pReadRear_ = &Array[Length];
	pWrite = pWriteHead_;
	pRead = pReadHead_;
	WriteInd = 0;
	ReadInd = 0;
	CircularListNum++;
}

template <typename Type>
CircularList<Type>::~CircularList()
{
	pWriteHead_ = NULL;
	pWriteRear_ = NULL;
	pReadHead_ = NULL;
	pReadRear_ = NULL;
	pWrite = NULL;
	pRead = NULL;
	pNextChain = NULL;
	pNextCircularChain = NULL;
	for(int i=0;i<Length_;i++)
	{
		Array[i].next = NULL;
	}
	delete[] Array;
}

template <typename Type>
int CircularList<Type>::SetNextChain(CircularList<Type>& NextChain)
{
	if(LinkStatus != Linked)
	{
		pNextChain = NextChain.GetArrayHeadPoint();
		Array[Length_-1].next = pNextChain;
		LinkStatus = Linked;
		return 0;
	}
	else
	{
		return -1;
	}
}

template <typename Type>
int CircularList<Type>::ChainLink(CircularList<Type>& NextChain)
{
	if((SetNextChain(NextChain) == 0)&&(NextChain.SetNextChain(*this) == 0))
	{
		SetNextFriendChain(NextChain);
		NextChain.SetNextFriendChain(*this);
		NextChain.WriteReset();
		WriteReset();
		return 0;
	}
	else
	{
		return -1;
	}
}

template <typename Type>
int CircularList<Type>::ChainLinkCheck()
{
	CircularList<Type>* pCheckList;
	if(LinkStatus == Linked)
	{
		pCheckList = pNextCircularChain;
		while(pCheckList != this)
		{
			if(pCheckList->LinkStatus != Linked)
			{
				return -2;
			}
			else if(pCheckList->LinkStatus == NULL)
			{
				return -3;
			}
			else
			{
				pCheckList = pCheckList->pNextCircularChain;
			}
		}
		return 0;
	}
	else
	{
		return -1;
	}
}

template <typename Type>
int CircularList<Type>::GetBufferCapacity()
{
	if((WRStatus == Writing)||(WRStatus == WriteEmpty))
	{
		if((Array[pWriteHead_->Index].next == pWriteHead_->next)&&(Array[pWriteRear_->Index].next == pWriteRear_->next))
		{
			if(pWriteHead_->Index > pWriteRear_->Index)
			{
				return pWriteRear_->Index - pWriteHead_->Index + Length_ + pNextCircularChain->GetLength() + 1;
			}
			else
			{
				return pWriteRear_->Index - pWriteHead_->Index + 1;
			}
		}
		else if((Array[pWriteHead_->Index].next == pWriteHead_->next)&&(Array[pWriteRear_->Index].next != pWriteRear_->next))
		{
			return Length_ - pWriteHead_->Index + pWriteRear_->Index + 1;
		}
		else if((Array[pWriteHead_->Index].next != pWriteHead_->next)&&(Array[pWriteRear_->Index].next == pWriteRear_->next))
		{
			return pNextCircularChain->GetLength() - pWriteHead_->Index + pWriteRear_->Index + 1;
		}
		else
		{
			if(pWriteHead_->Index > pWriteRear_->Index)
			{
				return pWriteRear_->Index - pWriteHead_->Index + Length_ + pNextCircularChain->GetLength() + 1;
			}
			else
			{
				return pWriteRear_->Index - pWriteHead_->Index + 1;
			}
		}
	}
	else
	{
		if((Array[pReadHead_->Index].next == pReadHead_->next)&&(Array[pReadRear_->Index].next == pReadRear_->next))
		{
			if(pReadHead_->Index > pReadRear_->Index)
			{
				return pReadRear_->Index - pReadHead_->Index + Length_ + pNextCircularChain->GetLength() + 1;
			}
			else
			{
				return pReadRear_->Index - pReadHead_->Index + 1;
			}
		}
		else if((Array[pReadHead_->Index].next == pReadHead_->next)&&(Array[pReadRear_->Index].next != pReadRear_->next))
		{
			return Length_ - pReadHead_->Index + pReadRear_->Index + 1;
		}
		else if((Array[pReadHead_->Index].next != pReadHead_->next)&&(Array[pReadRear_->Index].next == pReadRear_->next))
		{
			return pNextCircularChain->GetLength() - pReadHead_->Index + pReadRear_->Index + 1;
		}
		else
		{
			if(pReadHead_->Index > pReadRear_->Index)
			{
				return pReadRear_->Index - pReadHead_->Index + Length_ + pNextCircularChain->GetLength() + 1;
			}
			else
			{
				return pReadRear_->Index - pReadHead_->Index + 1;
			}
		}
	}
}

template <typename Type>
void CircularList<Type>::CapacityReset(const int Length)
{
	WRStatus = WriteEmpty;
	delete[] Array;
	Length_ = Length + 1;
	Array = new ListNode<Type>[Length_];
	memset(Array, 0, sizeof(ListNode<Type>)*Length_);
	for(int i = 0; i < Length; i++)
	{
		Array[i].Index = i;
		Array[i].next = &Array[i+1];
	}
	Array[Length].Index = Length;
	Array[Length].next = NULL;
	pNextChain 	= NULL;
	pWriteHead_ = &Array[0];
	pWriteRear_ = &Array[Length];
	pReadHead_ 	= &Array[0];
	pReadRear_ 	= &Array[Length];
	pWrite 		= pWriteHead_;
	pRead 		= pReadHead_;
	WriteInd 	= 0;
	ReadInd 	= 0;
	LinkStatus 	= Opening;
	pNextCircularChain->LinkStatus = Opening;
	pNextCircularChain->ChainLink(*this);
}

template <typename Type>
void CircularList<Type>::WriteReset()
{
	if(WRStatus == Reading)
	{
		WRStatus = Writing;
		pWrite = pNextCircularChain->pWriteRear_;
		WriteInd = 0;
		pWriteHead_ = pNextCircularChain->pWriteRear_->next;
		pWriteRear_ = pRead;
	}
	else
	{
		WRStatus = Writing;
		pWrite = pNextCircularChain->pWriteRear_;
		WriteInd = 0;
		pRead = pWrite;
		pNextCircularChain->pRead = pRead;
	}
}

template <typename Type>
void CircularList<Type>::WriteReset(int Ind)
{
	WRStatus = Writing;
	pWrite = pNextCircularChain->pWriteRear_;
	WriteInd = 0;
	pWriteHead_ = pNextCircularChain->pWriteRear_->next;
	if(Ind != ReadInd-1)
	{
		if(Ind > GetBufferCapacity()-1)
		{
			pRead = pReadRear_;
		}
		else
		{
			int RealInd;
			RealInd = Ind + pReadHead_->Index;
			if(Array[pReadHead_->Index].next == pReadHead_->next)
			{
				if(RealInd < Length_)
				{
					pRead = &Array[RealInd];
				}
				else if(RealInd < Length_ + pNextCircularChain->GetLength())
				{
					pRead = &(pNextCircularChain->Array[RealInd-Length_]);
				}
				else
				{
					pRead = &(Array[RealInd - Length_ - pNextCircularChain->GetLength()]);
				}
			}
			else
			{
				if(RealInd < pNextCircularChain->GetLength())
				{
					pRead = &(pNextCircularChain->Array[RealInd]);
				}
				else if(RealInd < Length_ + pNextCircularChain->GetLength())
				{
					pRead = &(Array[RealInd-pNextCircularChain->GetLength()]);
				}
				else
				{
					pRead = &(pNextCircularChain->Array[RealInd - Length_ - pNextCircularChain->GetLength()]);
				}
			}
		}
	}
	pWriteRear_ = pRead;
}

template <typename Type>
void CircularList<Type>::ReadReset()
{
	WRStatus = Reading;
	pReadHead_ = pNextCircularChain->pRead->next;
	pRead = pNextCircularChain->pRead;
	pReadRear_ = pWriteRear_;
	ReadInd = 0;
}

template <typename Type>
Type & CircularList<Type>::operator[](const int Ind)
{
	if(WRStatus == Writing)
	{
		if(Ind == WriteInd)
		{
			WriteInd++;
			pWrite = pWrite->next;
			return pWrite->data;
		}
		else
		{
			if(Ind > GetBufferCapacity()-1)
			{
				return pWriteRear_->data;
			}
			int RealInd;
			RealInd = Ind + pWriteHead_->Index;
			if(Array[pWriteHead_->Index].next == pWriteHead_->next)
			{
				if(RealInd < Length_)
				{
					return Array[RealInd].data;
				}
				else if(RealInd < Length_ + pNextCircularChain->GetLength())
				{
					return pNextCircularChain->Array[RealInd-Length_].data;
				}
				else
				{
					return Array[RealInd - Length_ - pNextCircularChain->GetLength()].data;
				}
			}
			else
			{
				if(RealInd < pNextCircularChain->GetLength())
				{
					return pNextCircularChain->Array[RealInd].data;
				}
				else if(RealInd < Length_ + pNextCircularChain->GetLength())
				{
					return Array[RealInd-pNextCircularChain->GetLength()].data;
				}
				else
				{
					return pNextCircularChain->Array[RealInd - Length_ - pNextCircularChain->GetLength()].data;
				}
			}
		}
	}
	else
	{
		if(Ind == ReadInd)
		{
			ReadInd++;
			pRead = pRead->next;
			return pRead->data;
		}
		else
		{
			if(Ind > GetBufferCapacity()-1)
			{
				return pReadRear_->data;
			}
			int RealInd;
			RealInd = Ind + pReadHead_->Index;
			if(Array[pReadHead_->Index].next == pReadHead_->next)
			{
				if(RealInd < Length_)
				{
					return Array[RealInd].data;
				}
				else if(RealInd < Length_ + pNextCircularChain->GetLength())
				{
					return pNextCircularChain->Array[RealInd-Length_].data;
				}
				else
				{
					return Array[RealInd - Length_ - pNextCircularChain->GetLength()].data;
				}
			}
			else
			{
				if(RealInd < pNextCircularChain->GetLength())
				{
					return pNextCircularChain->Array[RealInd].data;
				}
				else if(RealInd < Length_ + pNextCircularChain->GetLength())
				{
					return Array[RealInd-pNextCircularChain->GetLength()].data;
				}
				else
				{
					return pNextCircularChain->Array[RealInd - Length_ - pNextCircularChain->GetLength()].data;
				}
			}
		}
	}
}

template <typename Type>
void CircularList<Type>::show(const int Ind)
{
	COUTS("Array[", Ind, "].data = ", Array[Ind].data);
}

template <typename Type>
void CircularList<Type>::show(const int BeginInd ,const int EndInd)
{
	if(EndInd <= BeginInd)
	{
		COUTS("CircularList show Invalid!");
	}
	else
	{
		for(unsigned int i=0;i<EndInd-BeginInd+1;i++)
		{
			COUTS("Array[", BeginInd+i,"].data = ",Array[BeginInd+i].data);
		}
	}
}

#endif
