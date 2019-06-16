#include <Concurrent/Mutex.h>

#ifdef _WIN32

#include <Windows.h>
#include <concrt.h>

namespace Concurrent
{
	MutexPlatform::MutexPlatform()
	{
		entryCount.store(0);
	}

	MutexPlatform::~MutexPlatform()
	{
	}

	//////////////////////////////////////////////

	Mutex::Mutex()
		: MutexPlatform()
	{
	}

	Mutex::~Mutex()
	{
	}

	bool Mutex::lock()
	{
		try 
		{
			cs.lock();
		}
		catch (const Concurrency::improper_lock&)
		{
		}

		entryCount++;
		return true;
	}

	void Mutex::unlock()
	{
		if ( 0 == --entryCount )
			cs.unlock();
	}
}

#endif