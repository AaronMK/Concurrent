#ifndef _CONCURRENT_RW_LOCK_H_
#define _CONCURRENT_RW_LOCK_H_

#include "Config.h"
#include "Internal/RWLockPlatform.h"

namespace Concurrent
{
	/**
	 * @brief
	 *  Read/Write lock cooperative with the system threadpool. 
	 *
	 *  The Read/Write lock allows multiple readers, but only a single writer.
	 *  Writers are given exclusive and prioritized access, waiting for existing
	 *  readers to exit the protected critical section if necessary.
	 *
	 *  Recursive locking is supported with the following caveats:
	 *
	 *  - The recursive lock must have the same access as the lock already held by
	 *    the thread.  For example, obtaining a recursive read lock will only work
	 *    if the thread has a read lock, but not if it has a write lock.  Breaking
	 *    this rule results in deadlock.
	 *
	 *  - ReadLocker and WriteLocker objects must be destroyed in the reverse order
	 *    in which they were created.  Failing to do so could result in premature
	 *    releasing of the lock.
	 *
	 * @see
	 *  ReadLocker
	 *
	 * @see
	 *  WriteLocker
	 */
	class CONCURRENT_EXPORT RWLock : public RWLockPlatform
	{
		friend class ReadLocker;
		friend class WriteLocker;

	public:
		RWLock();
		virtual ~RWLock();
	};
}

#endif // CONCURRENT_RW_LOCK_H_