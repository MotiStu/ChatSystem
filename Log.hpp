#pragma once
#include<iostream>
#include<string>
using namespace std;
#define NORMAL 0
#define WARNING 1
#define ERROR 2
const char*arr[4]={"NORMAL","WARNING","ERROR",NULL};
#define LOG(str, level) log(str, level,__FILE__,__LINE__)

void log(string str,int level,string file,int line)
{
	cout<<str<<":"<<arr[level]<<","<<file<<","<<line <<endl;
}
