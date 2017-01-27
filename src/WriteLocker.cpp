#include <Concurrent/WriteLocker.h>

#ifdef _WIN32

namespace Concurrent
{
	WriteLocker::WriteLocker(RWLock *lock)
		: mLock(lock)
	{
		try
		{
			mLock->mPlatformLock.lock();
		}
		catch (Concurrency::improper_lock ex)
		{
			mLock = nullptr;
		}
	}

	WriteLocker::~WriteLocker()
	{
		if (nullptr != mLock)
			mLock->mPlatformLock.unlock();
	}
}

#endif // _WIN32