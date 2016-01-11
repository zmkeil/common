
/***********************************************
  File name		: pthread_util.cpp
  Create date	: 2016-01-11 20:49
  Modified date : 2016-01-11 20:50
  Author		: zmkeil
  Express : 
  
 **********************************************/

#include "clog.h"
#include "pthread_util.h"

namespace common {

ScopeGuard make_mutex_guard(pthread_mutex_t* mutex)
{
    return common::makeScopeGuard(
        [&mutex] () {
            if (pthread_mutex_lock(mutex) != 0) {
                CLOG(CALERT, "lock mutex error: %s", strerror(errno));
                pthread_exit((void*)"get mutex error");
            }
        },
        [&mutex] () {pthread_mutex_unlock(mutex);}
    );
}

}
