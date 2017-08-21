#include"DebugPrint.h"

#include<iostream>

__My_Debug::~__My_Debug() {
	std::cout<<(this->out.str().c_str())<<std::endl;
}