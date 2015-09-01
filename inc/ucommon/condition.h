// Copyright (C) 2006-2014 David Sugar, Tycho Softworks.
// Copyright (C) 2015 Cherokees of Idaho.
//
// This file is part of GNU uCommon C++.
//
// GNU uCommon C++ is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// GNU uCommon C++ is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with GNU uCommon C++.  If not, see <http://www.gnu.org/licenses/>.

/**
 * Thread classes and sychronization objects.
 * The theory behind ucommon thread classes is that they would be used
 * to create derived classes where thread-specific data can be stored as
 * member data of the derived class.  The run method is called when the
 * context is executed.  Since we use a pthread foundation, we support
 * both detached threads and joinable threads.  Objects based on detached
 * threads should be created with new, and will automatically delete when
 * the thread context exits.  Joinable threads will be joined with deleted.
 *
 * The theory behind ucommon sychronization objects is that all upper level
 * sychronization objects can be formed directly from a mutex and conditional.
 * This includes semaphores, barriers, rwlock, our own specialized conditional
 * lock, resource-bound locking, and recurive exclusive locks.  Using only
 * conditionals means we are not dependent on platform specific pthread
 * implimentations that may not impliment some of these, and hence improves
 * portability and consistency.  Given that our rwlocks are recursive access
 * locks, one can safely create read/write threading pairs where the read
 * threads need not worry about deadlocks and the writers need not either if
 * they only write-lock one instance at a time to change state.
 * @file ucommon/thread.h
 */

/**
 * An example of the thread queue class.  This may be relevant to producer-
 * consumer scenarios and realtime applications where queued messages are
 * stored on a re-usable object pool.
 * @example queue.cpp
 */

/**
 * A simple example of threading and join operation.
 * @example thread.cpp
 */

#ifndef _UCOMMON_CONDITION_H_
#define _UCOMMON_CONDITION_H_

#ifndef _UCOMMON_CPR_H_
#include <ucommon/cpr.h>
#endif

#ifndef _UCOMMON_ACCESS_H_
#include <ucommon/access.h>
#endif

#ifndef _UCOMMON_TIMERS_H_
#include <ucommon/timers.h>
#endif

#ifndef _UCOMMON_MEMORY_H_
#include <ucommon/memory.h>
#endif

namespace ucommon {

/**
 * The conditional is a common base for other thread synchronizing classes.
 * Many of the complex sychronization objects, including barriers, semaphores,
 * and various forms of read/write locks are all built from the conditional.
 * This assures that the minimum functionality to build higher order thread
 * synchronizing objects is a pure conditional, and removes dependencies on
 * what may be optional features or functions that may have different
 * behaviors on different pthread implimentations and platforms.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT Conditional
{
private:
    friend class ConditionalAccess;

#if defined(_MSTHREADS_)
    mutable CRITICAL_SECTION mutex;
    mutable CONDITION_VARIABLE cond;
#else
#ifndef __PTH__
    class __LOCAL attribute
    {
    public:
        pthread_condattr_t attr;
        attribute();
    };

    __LOCAL static attribute attr;
#endif

    mutable pthread_cond_t cond;
    mutable pthread_mutex_t mutex;
#endif

protected:
    friend class TimedEvent;

    /**
     * Conditional wait for signal on millisecond timeout.
     * @param timeout in milliseconds.
     * @return true if signalled, false if timer expired.
     */
    bool wait(timeout_t timeout);

    /**
     * Conditional wait for signal on timespec timeout.
     * @param timeout as a high resolution timespec.
     * @return true if signalled, false if timer expired.
     */
    bool wait(struct timespec *timeout);

#ifdef  _MSTHREADS_
    inline void lock(void)
        {EnterCriticalSection(&mutex);};

    inline void unlock(void)
        {LeaveCriticalSection(&mutex);};

    void wait(void);
    void signal(void);
    void broadcast(void);

#else
    /**
     * Lock the conditional's supporting mutex.
     */
    inline void lock(void)
        {pthread_mutex_lock(&mutex);}

    /**
     * Unlock the conditional's supporting mutex.
     */
    inline void unlock(void)
        {pthread_mutex_unlock(&mutex);}

    /**
     * Wait (block) until signalled.
     */
    inline void wait(void)
        {pthread_cond_wait(&cond, &mutex);}

    /**
     * Signal the conditional to release one waiting thread.
     */
    inline void signal(void)
        {pthread_cond_signal(&cond);}

    /**
     * Signal the conditional to release all waiting threads.
     */
    inline void broadcast(void)
        {pthread_cond_broadcast(&cond);}
#endif

    /**
     * Initialize and construct conditional.
     */
    Conditional();

    /**
     * Destroy conditional, release any blocked threads.
     */
    ~Conditional();

    friend class autolock;

public:
    class __EXPORT autolock
    {
    private:
#ifdef  _MSTHREADS_
        CRITICAL_SECTION *mutex;
#else
        pthread_mutex_t *mutex;
#endif

    public:
        inline autolock(const Conditional* object) {
            mutex = &object->mutex;
#ifdef _MSTHREADS_
            EnterCriticalSection(mutex);
#else
            pthread_mutex_lock(mutex);
#endif
        }

        inline ~autolock() {
#ifdef  _MSTHREADS_
            LeaveCriticalSection(mutex);
#else
            pthread_mutex_unlock(mutex);
#endif
        }
    };

#if !defined(_MSTHREADS_) && !defined(__PTH__)
    /**
     * Support function for getting conditional attributes for realtime
     * scheduling.
     * @return attributes to use for creating realtime conditionals.
     */
    static inline pthread_condattr_t *initializer(void)
        {return &attr.attr;}
#endif

    /**
     * Convert a millisecond timeout into use for high resolution
     * conditional timers.
     * @param hires timespec representation to set.
     * @param timeout to convert.
     */
    static void set(struct timespec *hires, timeout_t timeout);
};

/**
 * The conditional rw seperates scheduling for optizming behavior or rw locks.
 * This varient of conditonal seperates scheduling read (broadcast wakeup) and
 * write (signal wakeup) based threads.  This is used to form generic rwlock's
 * as well as the specialized condlock.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT ConditionalAccess : private Conditional
{
protected:
#if defined _MSTHREADS_
    CONDITION_VARIABLE bcast;
#else
    mutable pthread_cond_t bcast;
#endif

    unsigned pending, waiting, sharing;

    /**
     * Conditional wait for signal on millisecond timeout.
     * @param timeout in milliseconds.
     * @return true if signalled, false if timer expired.
     */
    bool waitSignal(timeout_t timeout);

    /**
     * Conditional wait for broadcast on millisecond timeout.
     * @param timeout in milliseconds.
     * @return true if signalled, false if timer expired.
     */
    bool waitBroadcast(timeout_t timeout);


    /**
     * Conditional wait for signal on timespec timeout.
     * @param timeout as a high resolution timespec.
     * @return true if signalled, false if timer expired.
     */
    bool waitSignal(struct timespec *timeout);

    /**
     * Conditional wait for broadcast on timespec timeout.
     * @param timeout as a high resolution timespec.
     * @return true if signalled, false if timer expired.
     */
    bool waitBroadcast(struct timespec *timeout);

    /**
     * Convert a millisecond timeout into use for high resolution
     * conditional timers.
     * @param hires timespec representation to set.
     * @param timeout to convert.
     */
    inline static void set(struct timespec *hires, timeout_t timeout)
        {Conditional::set(hires, timeout);}


#ifdef  _MSTHREADS_
    inline void lock(void)
        {EnterCriticalSection(&mutex);};

    inline void unlock(void)
        {LeaveCriticalSection(&mutex);};

    void waitSignal(void);

    void waitBroadcast(void);

    inline void signal(void)
        {Conditional::signal();};

    inline void broadcast(void)
        {Conditional::broadcast();};

#else
    /**
     * Lock the conditional's supporting mutex.
     */
    inline void lock(void)
        {pthread_mutex_lock(&mutex);}

    /**
     * Unlock the conditional's supporting mutex.
     */
    inline void unlock(void)
        {pthread_mutex_unlock(&mutex);}

    /**
     * Wait (block) until signalled.
     */
    inline void waitSignal(void)
        {pthread_cond_wait(&cond, &mutex);}

    /**
     * Wait (block) until broadcast.
     */
    inline void waitBroadcast(void)
        {pthread_cond_wait(&bcast, &mutex);}


    /**
     * Signal the conditional to release one signalled thread.
     */
    inline void signal(void)
        {pthread_cond_signal(&cond);}

    /**
     * Signal the conditional to release all broadcast threads.
     */
    inline void broadcast(void)
        {pthread_cond_broadcast(&bcast);}
#endif
public:
    /**
     * Initialize and construct conditional.
     */
    ConditionalAccess();

    /**
     * Destroy conditional, release any blocked threads.
     */
    ~ConditionalAccess();

    /**
     * Access mode shared thread scheduling.
     */
    void access(void);

    /**
     * Exclusive mode write thread scheduling.
     */
    void modify(void);

    /**
     * Release access mode read scheduling.
     */
    void release(void);

    /**
     * Complete exclusive mode write scheduling.
     */
    void commit(void);

    /**
     * Specify a maximum sharing (access) limit.  This can be used
     * to detect locking errors, such as when aquiring locks that are
     * not released.
     * @param max sharing level.
     */
    void limit_sharing(unsigned max);
};

/**
 * An optimized and convertable shared lock.  This is a form of read/write
 * lock that has been optimized, particularly for shared access.  Support
 * for scheduling access around writer starvation is also included.  The
 * other benefits over traditional read/write locks is that the code is
 * a little lighter, and read (shared) locks can be converted to exclusive
 * (write) locks to perform brief modify operations and then returned to read
 * locks, rather than having to release and re-aquire locks to change mode.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT ConditionalLock : protected ConditionalAccess, public SharedAccess
{
protected:
    class Context : public LinkedObject
    {
    public:
        inline Context(LinkedObject **root) : LinkedObject(root) {}

        pthread_t thread;
        unsigned count;
    };

    LinkedObject *contexts;

    virtual void _share(void);
    virtual void _unlock(void);

    Context *getContext(void);

public:
    /**
     * Construct conditional lock for default concurrency.
     */
    ConditionalLock();

    /**
     * Destroy conditional lock.
     */
    ~ConditionalLock();

    /**
     * Acquire write (exclusive modify) lock.
     */
    void modify(void);

    /**
     * Commit changes / release a modify lock.
     */
    void commit(void);

    /**
     * Acquire access (shared read) lock.
     */
    void access(void);

    /**
     * Release a shared lock.
     */
    void release(void);

    /**
     * Convert read lock into exclusive (write/modify) access.  Schedule
     * when other readers sharing.
     */
    virtual void exclusive(void);

    /**
     * Return an exclusive access lock back to share mode.
     */
    virtual void share(void);
};

/**
 * Convenience type for using conditional locks.
 */
typedef ConditionalLock condlock_t;

/**
 * Convenience type for scheduling access.
 */
typedef ConditionalAccess accesslock_t;

} // namespace ucommon

#endif