#ifndef _CONCURRENT_READ_LOCKER_H_
#define _CONCURRENT_READ_LOCKER_H_

#include "RWLock.h"

namespace Concurrent
{
	/**
	 * @brief
	 *  Class for scope based read locking of an RWLock.
	 */
	class CONCURRENT_EXPORT ReadLocker
	{
	public:
		ReadLocker(const ReadLocker&) = delete;
		ReadLocker& operator=(const ReadLocker&) = delete;

		/**
		 * @brief
		 *  Acquires the passed lock for reading, blocking until it is ready to read.
		 */
		ReadLocker(RWLock *lock);
		
		/**
		 * @brief
		 *  Releases ownership of the lock.
		 */
		virtual ~ReadLocker();

	private:
		#ifdef _WIN32
		Concurrency::reader_writer_lock::scoped_lock_read mLock;
		#endif
	};
}

#endif // _CONCURRENT_READ_LOCKER_H_