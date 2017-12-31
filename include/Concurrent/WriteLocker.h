#ifndef _CONCURRENT_WRITE_LOCKER_H_
#define _CONCURRENT_WRITE_LOCKER_H_

#include "RWLock.h"

namespace Concurrent
{
	/**
	 * @brief
	 *  Class for scope based write locking of an RWLock.
	 */
	class CONCURRENT_EXPORT WriteLocker
	{
	public:
		WriteLocker(const WriteLocker&) = delete;
		WriteLocker& operator=(const WriteLocker&) = delete;

		/**
		 * @brief
		 *  Acquires the passed lock for writing, blocking until it has ownership.
		 */
		WriteLocker(RWLock *lock);
		
		/**
		 * @brief
		 *  Releases ownership of the lock.
		 */
		virtual ~WriteLocker();

	private:
		#ifdef _WIN32
		Concurrency::reader_writer_lock::scoped_lock mLock;
		#endif
	};
}

#endif // _CONCURRENT_WRITE_LOCKER_H_