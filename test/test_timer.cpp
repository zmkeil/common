
/***********************************************
  File name		: test_timer.cpp
  Create date	: 2016-01-21 14:40
  Modified date	: 2016-01-21 14:40
  Author		: zmkeil, alibaba.inc
  Express : 
  
 **********************************************/

#include <iostream>
#include <string>
#include "timer.h"

int main()
{
	printf("now is: %s\n", common::asctime());

	std::string t;
	t.reserve(1024);
	common::local_time(&t);
	std::cout << t << std::endl;

	return 0;
}
