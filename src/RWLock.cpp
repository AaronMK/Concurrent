#include <Concurrent/RWLock.h>

#include <assert.h>

#ifdef _WIN32

namespace Concurrent
{
	
	RWLockPlatform::RWLockPlatform()
	{

	}

	RWLockPlatform::~RWLockPlatform()
	{

	}

	//////////////////////////////////////////

	RWLock::RWLock()
	{
	}

	RWLock::~RWLock()
	{
	}
}

#endif