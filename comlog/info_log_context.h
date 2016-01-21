#ifndef COMMON_LOG_INFO_LOG_CONTEXT_H
#define COMMON_LOG_INFO_LOG_CONTEXT_H

#include <stdarg.h>
#include "stderr_log.h"
#include "timer.h"

/*
 * InfoLogContext is used to info Runtime Messages, it use a AbstractLog as log_handler.
 * It will use a StderrLog and print messages to stderr by default
 *
 * Usage:
 * 		LOG(LEVEL, "XX", ARGS);		
 *
 * If you want to use selfdefine log, please implement AbstractLog, as follows:
 * 		class xxLog : public AbstractLog {}
 * 		xxLog* _log = new xxLog();
 * 		InfoLogContext::set_log(_log);
 * 		LOG(LEVEL, "XX", ARGS);
 */

#define LOG(_level_, _fmt_, args...)                                        \
    common::InfoLogContext::get_context()->log(_level_, "[%s:%d][%s] "  \
            _fmt_, __FILE__, __LINE__,  \
            __FUNCTION__, ##args)                                           \

namespace common {

class InfoLogContext;
extern InfoLogContext* info_log_context;

class InfoLogContext
{
public:
	static void set_log(AbstractLog* log) {
		if (!log) {
			fprintf(stderr, "InfoLogContext::set_log(NULL)");
			exit(-1);
		}
		get_context()->_log = log;
	}

    static InfoLogContext* get_context() {
        if (!info_log_context) {
            info_log_context = new InfoLogContext();
        }
        return info_log_context;
    }

public:
	// the API called by user to log messages
    void log(int level, const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		_log->comlog_write(level, fmt, args);
		va_end(args);
	}

private:
    InfoLogContext() {
		// the default log_handler, write to stderr with DEBUG level
        _log = new StderrLog();
    }

private:
    AbstractLog* _log;
};

}
#endif
