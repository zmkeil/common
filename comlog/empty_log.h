
/***********************************************
  File name		: empty_log.h
  Create date	: 2016-01-21 15:12
  Modified date	: 2016-01-21 15:12
  Author		: zmkeil, alibaba.inc
  Express : 
  
 **********************************************/
#ifndef COMMON_LOG_EMPTY_LOG_H
#define COMMON_LOG_EMPTY_LOG_H

#include "abstract_log.h"

namespace common {

class EmptyLog : public AbstractLog
{
public:
	EmptyLog() {
		_level = DEBUG;
	}
	virtual ~EmptyLog() {}

protected:
	void comlog_write_core(int level, const char* fmt, va_list args);
};

}
#endif
