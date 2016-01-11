#ifndef COMMON_SCOPE_GUARD_H
#define COMMON_SCOPE_GUARD_H
#include <memory>
#include <cstdint>
#include <utility>
#include <type_traits>

namespace common {
/**
 * ScopeGuard is a general implementation of the "Initialization is
 * Resource Acquisition" idiom.  Basically, it guarantees that a function
 * is executed upon leaving the currrent scope unless otherwise told.
 *
 * The makeScopeGuard() function is used to create a new ScopeGuard object.
 * It can be instantiated with a lambda function, a std::function<void()>,
 * a functor, or a function pointer.
 *
 * ************************************************************************************
 * Stolen from:
 *   Andrei's and Petru Marginean's CUJ article:
 *     http://drdobbs.com/184403758
 *   and the loki library:
 *     http://loki-lib.sourceforge.net/index.php?n=Idioms.ScopeGuardPointer
 *   and triendl.kj article:
 *     http://www.codeproject.com/KB/cpp/scope_guard.aspx
 *   and stackoverflow:
 *      http://stackoverflow.com/questions/10270328/the-simplest-and-neatest-c11-scopeguard
 *   and Facebook folly: ScopeGuard.h
 *
 * ************************************************************************************
 * Usage example: Add a friend to memory iff it is also added to the db.
 *
 * void User::addFriend(User& newFriend) {
 *   // add the friend to memory
 *   friends_.push_back(&newFriend);
 *
 *   // If the db insertion that follows fails, we should
 *   // remove it from memory.
 *   // (You could also declare this as "auto guard = makeScopeGuard(...)")
 *   ScopeGuard guard = makeScopeGuard([&] { friends_.pop_back(); });
 *
 *   // this will throw an exception upon error, which
 *   // makes the ScopeGuard execute User::pop_back()
 *   // once the Guard's destructor is called.
 *   db_->addFriend(GetName(), newFriend.GetName());
 *
 *   // an exception was not thrown, so don't execute
 *   // the Guard.
 *   guard.dismiss();
 * }
 *
 * Example 2:
 * {
 *      bthread_mutex_t mutex;
 *      // Initialize the mutex
 *      // ...
 *      // Get the lock:
 *      auto guard = makeScopeGuard(
 *          // lock the mutex in constructor
 *          [&mutex] {bthread_mutex_lock(&mutex);},
 *          // unlock the mutex in destructor.
 *          [&mutex] {bthread_mutex_unlock(&mutex);});
 *      // blabla...
 * }
 *
 * Example 3:
 * {
 *      // Initialize mysql library
 *      auto guard = makeScopeGuard(
 *          std::bind(::mysql_library_init, 0, nullptr, nullptr),
 *          ::mysql_library_end);
 *      // Mysql library initialized, start to build connection to mysql:)
 * }
 *
 */
class ScopeGuardImplBase {
public:
    ScopeGuardImplBase(ScopeGuardImplBase&& other) noexcept
            : _dismissed(other._dismissed) {
        other._dismissed = true;
    }

    void dismiss() noexcept {
        _dismissed = true;
    }
protected:
    ScopeGuardImplBase() : _dismissed(false) {}
    bool _dismissed;
};

// You can use either
//      ScopeGuard guard = makeScopeGuard(...);
// or
//      auto guard = makeScopeGuard(...);
// We recommend to use auto.
typedef ScopeGuardImplBase&& ScopeGuard;

template<typename FunctionType>
class ScopeGuardImpl : public ScopeGuardImplBase {
public:
    explicit ScopeGuardImpl(FunctionType&& fn)
            : _function(std::move(fn)) {}

    explicit ScopeGuardImpl(FunctionType& fn)
            : _function(fn) {}

    template<typename AcquireType, typename ReleaseType>
    ScopeGuardImpl(AcquireType&& acquire, ReleaseType&& release) noexcept
            : _function(std::move(release)) {
        std::forward<AcquireType>(acquire)();
    }

    template<typename AcquireType, typename ReleaseType>
    ScopeGuardImpl(AcquireType& acquire, ReleaseType& release) noexcept
            : _function(release) {
        acquire();
    }

    ScopeGuardImpl(ScopeGuardImpl&& other)
            : ScopeGuardImplBase(std::move(other)),
            _function(std::move(other._function)) {}

    ~ScopeGuardImpl() noexcept {
        if (!_dismissed) {
            execute();
        }
    }
private:
    void* operator new(size_t);
    void execute() noexcept {_function();}
    FunctionType _function;
};

template<typename FunctionType>
ScopeGuardImpl<typename std::decay<FunctionType>::type>
makeScopeGuard(FunctionType&& fn) {
    return ScopeGuardImpl<typename std::decay<FunctionType>::type>(
                    std::forward<FunctionType>(fn));
}

template<typename AcquireType, typename ReleaseType>
ScopeGuardImpl<typename std::decay<ReleaseType>::type>
makeScopeGuard(AcquireType&& acquire, ReleaseType&& release) noexcept {
    return ScopeGuardImpl<typename std::decay<ReleaseType>::type>(
                    std::forward<AcquireType>(acquire),
                    std::forward<ReleaseType>(release));
}
}
#endif
