#ifndef _ENGINE_MUTEX_H_
#define _ENGINE_MUTEX_H_

#include "Config.h"

#include "Internal/MutexPlatform.h"

namespace Concurrent
{
	/**
	 * @brief
	 *  A mutex class that abstracts system specific cooperative implementations.
	 * 
	 *  A thread can lock a mutex multiple times, but must have a
	 *  corresponding unlock call for each lock call.  The Mutex 
	 *  is cooperative with the system thread pool.
	 *
	 *  See MutexLocker for scope based locking.
	 */
	class CONCURRENT_EXPORT Mutex : private MutexPlatform
	{
	public:
		Mutex(const Mutex&) = delete;
		Mutex& operator=(const Mutex&) = delete;

		Mutex();
		virtual ~Mutex();

		/**
		 * @brief
		 *  Locks the mutex to the current thread.  If locked by another thread, it will
		 *  block until the thread releases it.
		 */
		bool lock();

		/**
		 * @brief
		 *  Unlocks the mutex, waking a single other thread waiting on it.  It is an error
		 *  to call unlock if the calling thread does not hold the mutex.
		 */
		void unlock();
	};
};

#endif // _ENGINE_MUTEX_H_