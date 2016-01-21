
/***********************************************
  File name		: stderr_log.h
  Create date	: 2015-12-17 23:12
  Modified date : 2016-01-21 15:12
  Author		: zmkeil, alibaba.inc
  Express : 
    Usually, log module is first inited,
    if error, write into stderr
  
 **********************************************/
#ifndef COMMON_LOG_STDERR_LOG_H
#define COMMON_LOG_STDERR_LOG_H

#include "timer.h"
#include "abstract_log.h"

namespace common {

class StderrLog : public AbstractLog
{
public:
	StderrLog(int level) : AbstractLog(level) {
		// the default time_handler, return a char pointer
		_time_handler = &asctime;
	}
	StderrLog() : AbstractLog(DEBUG) {
		_time_handler = &asctime;
	}

	virtual ~StderrLog() {}

    void comlog_write_core(int level, const char* fmt, va_list args);
};

} // common
#endif
