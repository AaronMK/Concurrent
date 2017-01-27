#include <Concurrent/ReadLocker.h>

#ifdef _WIN32

namespace Concurrent
{
	ReadLocker::ReadLocker(RWLock *lock)
		: mLock(lock)
	{
		try
		{
			mLock->mPlatformLock.lock_read();
		}
		catch (Concurrency::improper_lock ex)
		{
			mLock = nullptr;
		}
	}

	ReadLocker::~ReadLocker()
	{
		if (nullptr != mLock)
			mLock->mPlatformLock.unlock();
	}
}

#endif // _WIN32