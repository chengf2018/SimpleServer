// libeventtest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "WorkServer.h"

#include <iostream>


int main(int argc, const char **argv)
{
	CWorkServer ser;
	
	ser.Initialize();
	ser.SetPort(4399);
	ser.Start(true);


	std::cin.get();
	return 0;
}