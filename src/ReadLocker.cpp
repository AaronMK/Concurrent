#include <Concurrent/ReadLocker.h>

#ifdef _WIN32

namespace Concurrent
{
	ReadLocker::ReadLocker(RWLock *lock)
		: mLock(lock)
	{
		LockState& state = *lock->mThreadState;

		if (state == LockState::Read || state == LockState::Write)
		{
			mLock = nullptr;
		}
		else
		{
			mLock->mPlatformLock.lock_read();
			state = LockState::Read;
		}
	}

	ReadLocker::~ReadLocker()
	{
		if (nullptr != mLock)
		{
			mLock->mPlatformLock.unlock();
			*mLock->mThreadState = LockState::None;
		}
	}
}

#endif // _WIN32