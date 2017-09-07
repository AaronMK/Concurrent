#include <Concurrent/WriteLocker.h>

#ifdef _WIN32

#include <stdexcept>

namespace Concurrent
{
	WriteLocker::WriteLocker(RWLock *lock)
		: mLock(lock)
	{
		LockState& state = *lock->mThreadState;

		if (state == LockState::Write)
		{
			mLock = nullptr;
		}
		else if (state == LockState::None)
		{
			mLock->mPlatformLock.lock();
			state = LockState::Write;
		}
		else
		{
			throw std::logic_error("Write locking cannot be obtained when the thread has a read lock.");
		}
	}

	WriteLocker::~WriteLocker()
	{
		if (nullptr != mLock)
		{
			mLock->mPlatformLock.unlock();
			*mLock->mThreadState = LockState::None;
		}
	}
}

#endif // _WIN32