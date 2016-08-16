#include <Concurrent/MutexLocker.h>

namespace Concurrent
{
	MutexLocker::MutexLocker(Mutex* M)
	: theMutex(M)
	{
		theMutex->lock();
	}

	MutexLocker::~MutexLocker()
	{
		theMutex->unlock();
	}
}