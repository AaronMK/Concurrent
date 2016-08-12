#include <Concurrent/RWLock.h>

#include <assert.h>

#ifdef _WIN32

namespace Concurrent
{
	RWLock::RWLock()
	{
	}

	RWLock::~RWLock()
	{
	}

	void RWLock::lockRead()
	{
		try
		{
			mPlatformLock.lock_read();
		}
		catch (...)
		{
			// Likely trying to lock when the thread has already acquired the lock.
			assert(false);
		}
	}

	void RWLock::lockWrite()
	{
		try
		{
			mPlatformLock.lock();
		}
		catch (...)
		{
			// Likely trying to lock when the thread has already acquired the lock.
			assert(false);
		}
	}

	void RWLock::unlock()
	{
		mPlatformLock.unlock();
	}
}

#endif