
/***********************************************
  File name		: empty_log.cpp
  Create date	: 2016-01-21 15:26
  Modified date	: 2016-01-21 15:26
  Author		: zmkeil, alibaba.inc
  Express : 
  
 **********************************************/
#include <stdio.h>
#include "empty_log.h"

namespace common {

void EmptyLog::comlog_write_core(int level, const char* fmt, va_list args)
{
	(void) level;
	(void) fmt;
	(void) args;
	printf("empty log handler\n");
	return;
}

}
