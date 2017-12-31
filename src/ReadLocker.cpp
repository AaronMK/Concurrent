#include <Concurrent/ReadLocker.h>

#ifdef _WIN32

namespace Concurrent
{
	ReadLocker::ReadLocker(RWLock *lock)
		: mLock(lock->mPlatformLock)
	{
	}

	ReadLocker::~ReadLocker()
	{
	}
}

#endif // _WIN32