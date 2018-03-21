#include <Concurrent/WriteLocker.h>

#ifdef _WIN32

#include <stdexcept>

namespace Concurrent
{
	WriteLocker::WriteLocker(RWLock *lock)
		: mLock(lock->mPlatformLock)
	{
	}
	
	WriteLocker::WriteLocker(RWLock& lock)
		: mLock(lock.mPlatformLock)
	{
	}

	WriteLocker::~WriteLocker()
	{
	}
}

#endif // _WIN32