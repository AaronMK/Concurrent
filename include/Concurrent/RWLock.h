#ifndef _CONCURRENT_RW_LOCK_H_
#define _CONCURRENT_RW_LOCK_H_

#include "Config.h"

#include "Internal/RWLockPlatform.h"

#include "ThreadLocalPtr.h"

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
	 *  - Read locks can always be obtained.  If a write lock is already held, thread
	 *    will still have exclusive access and be considered in the write state.  If a
	 *    read lock is already held, the read state will remain.
	 *
	 *  - Write locks can be obtained if the thread does not currently hold the lock
	 *    or recursively over another write lock.  Trying to get a write lock when the
	 *    thread already has a read lock will throw an exception.
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

	private:

		enum class ThreadState
		{
			None,
			Read,
			Write
		};

		ThreadLocal<ThreadState> mThreadState;
	};
}

#endif // CONCURRENT_RW_LOCK_H_