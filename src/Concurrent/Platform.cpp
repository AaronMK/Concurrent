#include "private_include/Platform.h"

#if defined(_WIN32)

#include <Windows.h>
#include <concrt.h>

#include <thread>

using namespace std;

namespace Concurrent
{
	void sysScheduleFunction(void (*func)(void*), void* param)
	{
		Concurrency::CurrentScheduler::ScheduleTask(func, param);
	}

	void sysRunAsThread(function<void()>&& func)
	{
		thread t(forward<function<void()>>(func));
		t.detach();
	}
}

#else
#	error Add platform support for system scheduling functions.
#endif