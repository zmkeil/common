
/***********************************************
  File name		: test_info_log_context.cpp
  Create date	: 2016-01-21 13:52
  Modified date	: 2016-01-21 13:52
  Author		: zmkeil, alibaba.inc
  Express : 
  
 **********************************************/

#include "stderr_log.h"
#include "empty_log.h"
#include "info_log_context.h"

int main()
{
	LOG(DEBUG, "debug msg");
	LOG(ALERT, "alert msg");

	printf("stdout msg\n");

	common::EmptyLog* empty_log = new common::EmptyLog();
	common::InfoLogContext::set_log(empty_log);
	LOG(ERROR, "hello world");

	return 0;
}
