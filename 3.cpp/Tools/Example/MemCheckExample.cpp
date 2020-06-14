#include"../MemCheck.hpp"

int main(int argc, char **argv)
{
    char    char_a;
    short   short_b;
    int     int_c;
    float   float_d;
    double  double_e;
    int     intArray_a[100];
    int     pos = 0;
    for(pos = 0; pos < 100; pos++)
    {
        intArray_a[pos] = pos;
    }
	MEMCHECK_INIT;
    MEMCHECK_BORL;
	MEMCHECK_ADDVAR(char_a);
	MEMCHECK_ADDVAR(short_b);
	MEMCHECK_ADDVAR(int_c);
	MEMCHECK_ADDVAR(float_d);
	MEMCHECK_ADDVAR(double_e);
	MEMCHECK_ADDARRAY(intArray_a, 100, &pos);
    pos = 200;
	MEMCHECK_OVERFLOWCHECK;
	MEMCHECK_PRINT;
}
