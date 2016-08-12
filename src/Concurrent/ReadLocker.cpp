#include <Concurrent/ReadLocker.h>

namespace Concurrent
{
	ReadLocker::ReadLocker(RWLock *lock)
	: m_lock(lock)
	{
		m_lock->lockRead();
	}

	ReadLocker::~ReadLocker()
	{
		m_lock->unlock();
	}
}