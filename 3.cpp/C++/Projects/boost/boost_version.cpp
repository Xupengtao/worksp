#include<iostream>
using namespace std;
#include<boost/version.hpp>
#include<boost/config.hpp>
int main()
{
    cout<<BOOST_VERSION<<endl;
    cout<<BOOST_LIB_VERSION<<endl;
    cout<<BOOST_PLATFORM<<endl;
    cout<<BOOST_STDLIB<<endl;
}