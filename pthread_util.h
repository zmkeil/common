#ifndef COMMON_PTHREAD_UTIL_H
#define COMMON_PTHREAD_UTIL_H

extern "C" {
#include "pthread.h"
#include "string.h"
}
#include "scope_guard.h"

namespace common {

// get mutex at the begin of call_chain, avoid deadlock between several functions
// in the call_chain
// but func can't be class member function!
template<typename R>
R run_with_pthread_mutex(pthread_mutex_t *mutex, R (*func)())
{
    auto mutex_scope = common::makeScopeGuard(
        [&mutex] () {
            if (pthread_mutex_lock(mutex) != 0) {
                CLOG(CALERT, "lock mutex error: %s", strerror(errno));
                pthread_exit((void*)"get mutex error");
            }
        },
        [&mutex] () {pthread_mutex_unlock(mutex);}
    );
    return func();
}

template<typename R, typename Arg1>
R run_with_pthread_mutex(pthread_mutex_t *mutex, R (*func)(Arg1), Arg1 arg1)
{
    auto mutex_scope = common::makeScopeGuard(
        [&mutex] () {
            if (pthread_mutex_lock(mutex) != 0) {
                CLOG(CALERT, "lock mutex error: %s", strerror(errno));
                pthread_exit((void*)"get mutex error");
            }
        },
        [&mutex] () {pthread_mutex_unlock(mutex);}
    );
    return func(arg1);
}


// get mutex at every call, used in class member function
ScopeGuard make_mutex_guard(pthread_mutex_t* mutex);

}
#endif
