/*
 * @Author: xpt 
 * @Date: 2020-05-19 21:21:29 
 * @Last Modified by:   xpt 
 * @Last Modified time: 2020-05-19 21:21:29 
 */

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