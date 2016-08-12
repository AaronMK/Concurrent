#ifndef _ENGINE_MUTEX_LOCKER_H_
#define _ENGINE_MUTEX_LOCKER_H_

#include "Mutex.h"

namespace Concurrent
{
	/**
	 * @brief
	 *  Scope based locking for a Mutex.
	 *
	 *  The constructor will take give ownership of the Mutex to
	 *  the current thread, and destructor will automatically release ownership.
	 */
	class CONCURRENT_DYNAMIC_CLASS MutexLocker
	{
	public:
		MutexLocker(const MutexLocker&) = delete;
		MutexLocker& operator=(const MutexLocker&) = delete;

		/**
		 * @brief
		 *  Contructs a locker that will take ownership of the passed Mutex.  This
		 *  call will block until the Mutex is available.
		 */
		MutexLocker(Mutex* M);

		/**
		 * @brief
		 *  Destructor automatically releases ownership of Mutex.
		 */
		virtual ~MutexLocker();

	private:
		Mutex* theMutex;
	};
}

#endif // _ENGINE_MUTEX_LOCKER_H_