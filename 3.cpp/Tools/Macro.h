/*
 * Macro.h
 *
 *  Created on: Jul 1, 2019
 *      Author: xupengtao
 */

#ifndef MACRO_H_
#define MACRO_H_

#include<iomanip>
#include<iostream>
using namespace std;

#ifndef MAGIC_MACRO
#define PRIVATE_ARGS_GLUE(x,y) 					x y
#define PRIVATE_MACRO_VAR_ARGS_IMPL_COUNT(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,N,...) N								
#define PRIVATE_MACRO_VAR_ARGS_IMPL(args) 		PRIVATE_MACRO_VAR_ARGS_IMPL_COUNT args							//VS2008
#define COUNT_MACRO_VAR_ARGS(...) 				PRIVATE_MACRO_VAR_ARGS_IMPL((__VA_ARGS__,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0))
#define PRIVATE_MACRO_CHOOSE_HELPER2(M,count) 	M##count
#define PRIVATE_MACRO_CHOOSE_HELPER1(M,count) 	PRIVATE_MACRO_CHOOSE_HELPER2(M,count)
#define PRIVATE_MACRO_CHOOSE_HELPER(M,count) 	PRIVATE_MACRO_CHOOSE_HELPER1(M,count)
#define INVOKE_VAR_MACRO(M,...) 				PRIVATE_ARGS_GLUE(PRIVATE_MACRO_CHOOSE_HELPER(M,COUNT_MACRO_VAR_ARGS(__VA_ARGS__)),(__VA_ARGS__))
#endif

#ifndef COUT
#define COUTFILE 				std::cout<<"File: \""<< __FILE__<<"\""
#define COUTLINE 				std::cout<<"(Line:"<< __LINE__<<")"
#define COUTSEC(SecName)		std::cout<<endl<<"|--------------------------------------------------------- "<<SecName<<" ---------------------------------------------------------|"<<endl
#define COUTSECBEG(SecName)		std::cout<<endl<<"|--------------------------------------------------------- "<<SecName<<" Begin"<<" -------------------------------------------------------|"<<endl
#define COUTSECEND(SecName)		std::cout<<endl<<"|--------------------------------------------------------- "<<SecName<<" End"<<" ---------------------------------------------------------|"<<endl
#define COUTSTR(STR)  			std::cout<<STR<<" ---> "
#define COUTSTREND(STR)  		std::cout<<STR<<endl
#define COUTFUNC(FuncName)  	std::cout<<FuncName<<" ---> "
#define COUTFUNCEND(FuncName)  	std::cout<<FuncName<<endl
#define COUTMSGVAL(ValName)   	std::cout<<"@Message: "#ValName<<" - "<<ValName<<" ---> "
#define COUTMSGVALEND(ValName)  std::cout<<"@Message: "#ValName<<" - "<<ValName<<endl
#define COUTMSGSTR(STR)   		std::cout<<"@Message: "<<STR<<" ---> "
#define COUTMSGSTREND(STR)   	std::cout<<"@Message: "<<STR<<endl
#define COUTEND   				std::cout<<"End"<<endl
#define COUTHEX   				std::cout<<hex
#define COUTDEC   				std::cout<<dec
#define COUTSHOWBASE   			std::cout<<showbase
#define COUTFILL(STR)   		std::cout<<setfill(STR)
#define COUTSETW(W)   			std::cout.width(W)
#define COUTUPPER   			std::cout<<uppercase
#define PRE_LINKSTRING	" - "
#define BACK_LINKSTRING "   "
#define COUT_VAL(x) 										#x<<PRE_LINKSTRING<<(x)<<BACK_LINKSTRING
#define PRINT_ARGS1(a1)										std::cout<<COUT_VAL(a1)<<std::endl
#define PRINT_ARGS2(a1,a2)									std::cout<<COUT_VAL(a1)<<COUT_VAL(a2)<<std::endl
#define PRINT_ARGS3(a1,a2,a3)								std::cout<<COUT_VAL(a1)<<COUT_VAL(a2)<<COUT_VAL(a3)<<std::endl
#define PRINT_ARGS4(a1,a2,a3,a4)							std::cout<<COUT_VAL(a1)<<COUT_VAL(a2)<<COUT_VAL(a3)<<COUT_VAL(a4)<<std::endl
#define PRINT_ARGS5(a1,a2,a3,a4,a5)							std::cout<<COUT_VAL(a1)<<COUT_VAL(a2)<<COUT_VAL(a3)<<COUT_VAL(a4)<<COUT_VAL(a5)<<std::endl
#define PRINT_ARGS6(a1,a2,a3,a4,a5,a6)						std::cout<<COUT_VAL(a1)<<COUT_VAL(a2)<<COUT_VAL(a3)<<COUT_VAL(a4)<<COUT_VAL(a5)<<COUT_VAL(a6)<<std::endl
#define PRINT_ARGS7(a1,a2,a3,a4,a5,a6,a7)					std::cout<<COUT_VAL(a1)<<COUT_VAL(a2)<<COUT_VAL(a3)<<COUT_VAL(a4)<<COUT_VAL(a5)<<COUT_VAL(a6)<<COUT_VAL(a7)<<std::endl
#define PRINT_ARGS8(a1,a2,a3,a4,a5,a6,a7,a8)				std::cout<<COUT_VAL(a1)<<COUT_VAL(a2)<<COUT_VAL(a3)<<COUT_VAL(a4)<<COUT_VAL(a5)<<COUT_VAL(a6)<<COUT_VAL(a7)<<COUT_VAL(a8)<<std::endl
#define PRINT_ARGS9(a1,a2,a3,a4,a5,a6,a7,a8,a9)				std::cout<<COUT_VAL(a1)<<COUT_VAL(a2)<<COUT_VAL(a3)<<COUT_VAL(a4)<<COUT_VAL(a5)<<COUT_VAL(a6)<<COUT_VAL(a7)<<COUT_VAL(a8)<<COUT_VAL(a9)<<std::endl
#define COUTWIDTH_VAL(w,x) 									setw((w))<<#x<<PRE_LINKSTRING<<setw((w))<<(x)<<BACK_LINKSTRING
#define PRINTWIDTH_ARGS2(w,a1)								std::cout<<left<<COUTWIDTH_VAL(w,a1)<<std::endl
#define PRINTWIDTH_ARGS3(w,a1,a2)							std::cout<<left<<COUTWIDTH_VAL(w,a1)<<COUTWIDTH_VAL(w,a2)<<std::endl
#define PRINTWIDTH_ARGS4(w,a1,a2,a3)						std::cout<<left<<COUTWIDTH_VAL(w,a1)<<COUTWIDTH_VAL(w,a2)<<COUTWIDTH_VAL(w,a3)<<std::endl
#define PRINTWIDTH_ARGS5(w,a1,a2,a3,a4)						std::cout<<left<<COUTWIDTH_VAL(w,a1)<<COUTWIDTH_VAL(w,a2)<<COUTWIDTH_VAL(w,a3)<<COUTWIDTH_VAL(w,a4)<<std::endl
#define PRINTWIDTH_ARGS6(w,a1,a2,a3,a4,a5)					std::cout<<left<<COUTWIDTH_VAL(w,a1)<<COUTWIDTH_VAL(w,a2)<<COUTWIDTH_VAL(w,a3)<<COUTWIDTH_VAL(w,a4)<<COUTWIDTH_VAL(w,a5)<<std::endl
#define PRINTWIDTH_ARGS7(w,a1,a2,a3,a4,a5,a6)				std::cout<<left<<COUTWIDTH_VAL(w,a1)<<COUTWIDTH_VAL(w,a2)<<COUTWIDTH_VAL(w,a3)<<COUTWIDTH_VAL(w,a4)<<COUTWIDTH_VAL(w,a5)<<COUTWIDTH_VAL(w,a6)<<std::endl
#define PRINTWIDTH_ARGS8(w,a1,a2,a3,a4,a5,a6,a7)			std::cout<<left<<COUTWIDTH_VAL(w,a1)<<COUTWIDTH_VAL(w,a2)<<COUTWIDTH_VAL(w,a3)<<COUTWIDTH_VAL(w,a4)<<COUTWIDTH_VAL(w,a5)<<COUTWIDTH_VAL(w,a6)<<COUTWIDTH_VAL(w,a7)<<std::endl
#define COUT(...) 											COUTDEC;INVOKE_VAR_MACRO(PRINT_ARGS,__VA_ARGS__)
#define COUTBYHEX(...) 						   COUTSHOWBASE;COUTHEX;INVOKE_VAR_MACRO(PRINT_ARGS,__VA_ARGS__)
#define COUTWIDTH(...) 										COUTDEC;INVOKE_VAR_MACRO(PRINTWIDTH_ARGS,__VA_ARGS__)
#define COUTBYHEXWIDTH(...)  				   COUTSHOWBASE;COUTHEX;INVOKE_VAR_MACRO(PRINTWIDTH_ARGS,__VA_ARGS__)										
#define COUT_STRTOVAL_VAL(s,x) 															s<<(x)
#define PRINT_STRTOVAL_ARGS1(s1)														std::cout<<s1<<std::endl
#define PRINT_STRTOVAL_ARGS2(s1,a1)														std::cout<<COUT_STRTOVAL_VAL(s1,a1)<<std::endl
#define PRINT_STRTOVAL_ARGS3(s1,a1,s2)													std::cout<<COUT_STRTOVAL_VAL(s1,a1)<<s2<<std::endl
#define PRINT_STRTOVAL_ARGS4(s1,a1,s2,a2)												std::cout<<COUT_STRTOVAL_VAL(s1,a1)<<COUT_STRTOVAL_VAL(s2,a2)<<std::endl
#define PRINT_STRTOVAL_ARGS5(s1,a1,s2,a2,s3)											std::cout<<COUT_STRTOVAL_VAL(s1,a1)<<COUT_STRTOVAL_VAL(s2,a2)<<s3<<std::endl
#define PRINT_STRTOVAL_ARGS6(s1,a1,s2,a2,s3,a3)											std::cout<<COUT_STRTOVAL_VAL(s1,a1)<<COUT_STRTOVAL_VAL(s2,a2)<<COUT_STRTOVAL_VAL(s3,a3)<<std::endl
#define PRINT_STRTOVAL_ARGS7(s1,a1,s2,a2,s3,a3,s4)										std::cout<<COUT_STRTOVAL_VAL(s1,a1)<<COUT_STRTOVAL_VAL(s2,a2)<<COUT_STRTOVAL_VAL(s3,a3)<<s4<<std::endl
#define PRINT_STRTOVAL_ARGS8(s1,a1,s2,a2,s3,a3,s4,a4)									std::cout<<COUT_STRTOVAL_VAL(s1,a1)<<COUT_STRTOVAL_VAL(s2,a2)<<COUT_STRTOVAL_VAL(s3,a3)<<COUT_STRTOVAL_VAL(s4,a4)<<std::endl
#define PRINT_STRTOVAL_ARGS9(s1,a1,s2,a2,s3,a3,s4,a4,s5)								std::cout<<COUT_STRTOVAL_VAL(s1,a1)<<COUT_STRTOVAL_VAL(s2,a2)<<COUT_STRTOVAL_VAL(s3,a3)<<COUT_STRTOVAL_VAL(s4,a4)<<s5<<std::endl
#define PRINT_STRTOVAL_ARGS10(s1,a1,s2,a2,s3,a3,s4,a4,s5,a5)							std::cout<<COUT_STRTOVAL_VAL(s1,a1)<<COUT_STRTOVAL_VAL(s2,a2)<<COUT_STRTOVAL_VAL(s3,a3)<<COUT_STRTOVAL_VAL(s4,a4)<<COUT_STRTOVAL_VAL(s5,a5)<<std::endl
#define PRINT_STRTOVAL_ARGS11(s1,a1,s2,a2,s3,a3,s4,a4,s5,a5,s6)							std::cout<<COUT_STRTOVAL_VAL(s1,a1)<<COUT_STRTOVAL_VAL(s2,a2)<<COUT_STRTOVAL_VAL(s3,a3)<<COUT_STRTOVAL_VAL(s4,a4)<<COUT_STRTOVAL_VAL(s5,a5)<<s6<<std::endl
#define PRINT_STRTOVAL_ARGS12(s1,a1,s2,a2,s3,a3,s4,a4,s5,a5,s6,a6)						std::cout<<COUT_STRTOVAL_VAL(s1,a1)<<COUT_STRTOVAL_VAL(s2,a2)<<COUT_STRTOVAL_VAL(s3,a3)<<COUT_STRTOVAL_VAL(s4,a4)<<COUT_STRTOVAL_VAL(s5,a5)<<COUT_STRTOVAL_VAL(s6,a6)<<std::endl
#define PRINT_STRTOVAL_ARGS13(s1,a1,s2,a2,s3,a3,s4,a4,s5,a5,s6,a6,s7)					std::cout<<COUT_STRTOVAL_VAL(s1,a1)<<COUT_STRTOVAL_VAL(s2,a2)<<COUT_STRTOVAL_VAL(s3,a3)<<COUT_STRTOVAL_VAL(s4,a4)<<COUT_STRTOVAL_VAL(s5,a5)<<COUT_STRTOVAL_VAL(s6,a6)<<s7<<std::endl
#define PRINT_STRTOVAL_ARGS14(s1,a1,s2,a2,s3,a3,s4,a4,s5,a5,s6,a6,s7,a7)				std::cout<<COUT_STRTOVAL_VAL(s1,a1)<<COUT_STRTOVAL_VAL(s2,a2)<<COUT_STRTOVAL_VAL(s3,a3)<<COUT_STRTOVAL_VAL(s4,a4)<<COUT_STRTOVAL_VAL(s5,a5)<<COUT_STRTOVAL_VAL(s6,a6)<<COUT_STRTOVAL_VAL(s7,a7)<<std::endl
#define PRINT_STRTOVAL_ARGS15(s1,a1,s2,a2,s3,a3,s4,a4,s5,a5,s6,a6,s7,a7,s8)				std::cout<<COUT_STRTOVAL_VAL(s1,a1)<<COUT_STRTOVAL_VAL(s2,a2)<<COUT_STRTOVAL_VAL(s3,a3)<<COUT_STRTOVAL_VAL(s4,a4)<<COUT_STRTOVAL_VAL(s5,a5)<<COUT_STRTOVAL_VAL(s6,a6)<<COUT_STRTOVAL_VAL(s7,a7)<<s8<<std::endl
#define COUT_STRTOVALWIDTH_VAL(w,s,x) 													s<<setw((w))<<(x)
#define PRINT_STRTOVALWIDTH_ARGS3(w,s1,a1)												std::cout<<left<<COUT_STRTOVALWIDTH_VAL(w,s1,a1)<<std::endl
#define PRINT_STRTOVALWIDTH_ARGS5(w,s1,a1,s2,a2)										std::cout<<left<<COUT_STRTOVALWIDTH_VAL(w,s1,a1)<<COUT_STRTOVALWIDTH_VAL(w,s2,a2)<<std::endl
#define PRINT_STRTOVALWIDTH_ARGS7(w,s1,a1,s2,a2,s3,a3)									std::cout<<left<<COUT_STRTOVALWIDTH_VAL(w,s1,a1)<<COUT_STRTOVALWIDTH_VAL(w,s2,a2)<<COUT_STRTOVALWIDTH_VAL(w,s3,a3)<<std::endl
#define PRINT_STRTOVALWIDTH_ARGS9(w,s1,a1,s2,a2,s3,a3,s4,a4)							std::cout<<left<<COUT_STRTOVALWIDTH_VAL(w,s1,a1)<<COUT_STRTOVALWIDTH_VAL(w,s2,a2)<<COUT_STRTOVALWIDTH_VAL(w,s3,a3)<<COUT_STRTOVALWIDTH_VAL(w,s4,a4)<<std::endl
#define PRINT_STRTOVALWIDTH_ARGS11(w,s1,a1,s2,a2,s3,a3,s4,a4,s5,a5)						std::cout<<left<<COUT_STRTOVALWIDTH_VAL(w,s1,a1)<<COUT_STRTOVALWIDTH_VAL(w,s2,a2)<<COUT_STRTOVALWIDTH_VAL(w,s3,a3)<<COUT_STRTOVALWIDTH_VAL(w,s4,a4)<<COUT_STRTOVALWIDTH_VAL(w,s5,a5)<<std::endl
#define PRINT_STRTOVALWIDTH_ARGS13(w,s1,a1,s2,a2,s3,a3,s4,a4,s5,a5,s6,a6)				std::cout<<left<<COUT_STRTOVALWIDTH_VAL(w,s1,a1)<<COUT_STRTOVALWIDTH_VAL(w,s2,a2)<<COUT_STRTOVALWIDTH_VAL(w,s3,a3)<<COUT_STRTOVALWIDTH_VAL(w,s4,a4)<<COUT_STRTOVALWIDTH_VAL(w,s5,a5)<<COUT_STRTOVALWIDTH_VAL(w,s6,a6)<<std::endl
#define PRINT_STRTOVALWIDTH_ARGS15(w,s1,a1,s2,a2,s3,a3,s4,a4,s5,a5,s6,a6,s7,a7)			std::cout<<left<<COUT_STRTOVALWIDTH_VAL(w,s1,a1)<<COUT_STRTOVALWIDTH_VAL(w,s2,a2)<<COUT_STRTOVALWIDTH_VAL(w,s3,a3)<<COUT_STRTOVALWIDTH_VAL(w,s4,a4)<<COUT_STRTOVALWIDTH_VAL(w,s5,a5)<<COUT_STRTOVALWIDTH_VAL(w,s6,a6)<<COUT_STRTOVALWIDTH_VAL(w,s7,a7)<<std::endl
#define PRINT_STRTOVALWIDTH_ARGS17(w,s1,a1,s2,a2,s3,a3,s4,a4,s5,a5,s6,a6,s7,a7,s8,a8)	std::cout<<left<<COUT_STRTOVALWIDTH_VAL(w,s1,a1)<<COUT_STRTOVALWIDTH_VAL(w,s2,a2)<<COUT_STRTOVALWIDTH_VAL(w,s3,a3)<<COUT_STRTOVALWIDTH_VAL(w,s4,a4)<<COUT_STRTOVALWIDTH_VAL(w,s5,a5)<<COUT_STRTOVALWIDTH_VAL(w,s6,a6)<<COUT_STRTOVALWIDTH_VAL(w,s7,a7)<<COUT_STRTOVALWIDTH_VAL(w,s8,a8)<<std::endl
#define COUTS(...) 																COUTDEC;INVOKE_VAR_MACRO(PRINT_STRTOVAL_ARGS,__VA_ARGS__)
#define COUTSBYHEX(...) 									   		COUTSHOWBASE;COUTHEX;INVOKE_VAR_MACRO(PRINT_STRTOVAL_ARGS,__VA_ARGS__)
#define COUTSWIDTH(...) 														COUTDEC;INVOKE_VAR_MACRO(PRINT_STRTOVALWIDTH_ARGS,__VA_ARGS__)
#define COUTSBYHEXWIDTH(...) 								   		COUTSHOWBASE;COUTHEX;INVOKE_VAR_MACRO(PRINT_STRTOVALWIDTH_ARGS,__VA_ARGS__)
#define COUT_STATUS(x)						\
	if((x) == 0)							\
	{										\
		std::cout<<#x<<" = IDLE"<<endl;		\
	}										\
	else if((x) == 1)						\
	{										\
		std::cout<<#x<<" = BUSY"<<endl;		\
	}										\
	else if((x) == 2)						\
	{										\
		std::cout<<#x<<" = READY"<<endl;	\
	}										\
	else									\
	{										\
		;									\
	}
#endif

#ifndef _ITERTYPE_TRAITS
#define _ITERTYPE_TRAITS
#define STR_VAL(x) x,#x"   "
#define TYPETRAIT(x) Typetrait(STR_VAL(x));							//�磺TYPETRAIT(a);
#define ITERATOR_TRAITS_STRING "Type traits:     "
#define TYPETRAIT_PRINT(x)							\
inline void Typetrait_print(x*)						\
{													\
	cout<<ITERATOR_TRAITS_STRING<<#x<<endl;			\
}
#define TEMPLATE_TYPETRAIT_PRINT(x)					\
template<class _Type,class _KeyTx,size_t _Size>		\
inline void Typetrait_print(x<_Type,_KeyTx,_Size>*)	\
{													\
	cout<<ITERATOR_TRAITS_STRING<<#x<<endl;			\
}
#endif

#endif /* MACRO_H_ */
