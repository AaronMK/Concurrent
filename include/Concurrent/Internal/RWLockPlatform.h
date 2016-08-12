#ifndef _CONCURRENT_RW_LOCK_PLATFORM_H_
#define _CONCURRENT_RW_LOCK_PLATFORM_H_

#include "../Config.h"

#ifdef _WIN32
#include <concrt.h>

namespace Concurrent
{
	/**
	 * @internal
	 */
	class CONCURRENT_DYNAMIC_CLASS RWLockPlatform
	{
	protected:
		Concurrency::reader_writer_lock mPlatformLock;
	};
}

#endif

#endif // _CONCURRENT_RW_LOCK_PLATFORM_H_