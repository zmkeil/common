
/***********************************************
  File name		: abstract_log.h
  Create date	: 2015-12-17 23:12
  Modified date : 2016-01-21 14:26
  Author		: zmkeil, alibaba.inc
  Express : 
    Usually, log module is first inited,
    if error, write into stderr
  
 **********************************************/
#ifndef COMMON_LOG_ABSTRACT_LOG_H
#define COMMON_LOG_ABSTRACT_LOG_H

#include <stdarg.h>

namespace common {

#define STDERR		0
#define EMERG       1
#define ALERT       2
#define ERROR       3
#define WARN        4
#define NOTICE      5
#define INFO        6
#define DEBUG       7

class AbstractLog 
{
public:
	AbstractLog() : _level(DEBUG) {}
	AbstractLog(int level) : _level(level) {}

    void comlog_write(int level, const char* fmt, va_list args) 
	{
		if (level <= _level) {
			comlog_write_core(level, fmt, args);
		} 
		return;
	}

    virtual void comlog_write_core(int level, const char* fmt, va_list args) = 0;

protected:
	// every line of log will print the level
    int _level;
	// If it isn't null, print the time
	char* (*_time_handler)();
};

} // ngxplus
#endif
