
/***********************************************
  File name		: stderr_log.cpp
  Create date	: 2015-12-17 23:28
  Modified date : 2016-01-21 14:48
  Author		: zmkeil, alibaba.inc
  Express : 
  
 **********************************************/

#include <stdio.h>
#include "stderr_log.h"

namespace common {
/* 
 * common Log
 * just printf message on stderr
 */

static const char *err_levels[] = {
    "STDERR",
    "emerg",
    "alert",
    "error",
    "warn",
    "notice",
    "info",
    "debug"
};

void StderrLog::comlog_write_core(int level, const char* fmt, va_list args)
{
	fprintf(stderr, "[clog %s] ", err_levels[level]);
	if (_time_handler) {
		fprintf(stderr, "%s ", _time_handler());
	} else {
		fprintf(stderr, "no_timer_handler ");
	}
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	return;
}

} // ngxplus
