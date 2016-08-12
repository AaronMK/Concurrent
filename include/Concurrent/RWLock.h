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
	 *  readers to exit the protected critical section if necessary.  Recursive
	 *  locking is not supported.
	 *
	 * @see
	 *  ReadLocker
	 *
	 * @see
	 *  WriteLocker
	 */
	class CONCURRENT_DYNAMIC_CLASS RWLock : public RWLockPlatform
	{
		friend class ReadLocker;
		friend class WriteLocker;

	public:
		RWLock();
		virtual ~RWLock();

	private:
		void lockRead();
		void lockWrite();
		void unlock();
	};
}

#endif // CONCURRENT_RW_LOCK_H_