#include <Concurrent/WriteLocker.h>

namespace Concurrent
{
	WriteLocker::WriteLocker(RWLock *lock)
	: m_lock(lock)
	{
		m_lock->lockWrite();
	}

	WriteLocker::~WriteLocker()
	{
		m_lock->unlock();
	}
}