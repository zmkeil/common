
/***********************************************
  File name		: timer.h
  Create date	: 2016-01-21 13:39
  Modified date : 2016-01-21 13:41
  Author		: zmkeil, alibaba.inc
  Express : 
  
 **********************************************/

#ifndef COMMON_TIMER_H
#define COMMON_TIMER_H

#include <string>

namespace common {

bool local_time(std::string* str_time);

char* asctime();

}

#endif
