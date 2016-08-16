#ifndef _CONCURRENT_PLATFORM_H_
#define _CONCURRENT_PLATFORM_H_

#include <functional>

namespace Concurrent
{
	/**
	 * Adds a function directly to the function queue of the platform scheduler.
	 */
	extern void sysScheduleFunction(void (*func)(void*), void* param);

	/**
	 * Runs the passed function as a thread.
	 */
	extern void sysRunAsThread(std::function<void()>&& func);
}

#endif // _CONCURRENT_PLATFORM_H_