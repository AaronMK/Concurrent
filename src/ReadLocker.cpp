#include <Concurrent/ReadLocker.h>

#ifdef _WIN32

namespace Concurrent
{
	ReadLocker::ReadLocker(RWLock *lock)
		: mLock(lock->mPlatformLock)
	{
	}

	ReadLocker::ReadLocker(RWLock& lock)
		: ReadLocker(&lock)
	{
	}

	ReadLocker::~ReadLocker()
	{
	}
}

#endif // _WIN32