#ifndef _CONCURRENT_QUEUE_PLATFORM_H_
#define _CONCURRENT_QUEUE_PLATFORM_H_

#include <Concurrent/Config.h>

#ifdef _WIN32

#include <concurrent_queue.h>

namespace Concurrent
{
	template<typename T>
	using QueuePlatform = concurrency::concurrent_queue<T>;
}

#endif

#endif // _CONCURRENT_QUEUE_PLATFORM_H_