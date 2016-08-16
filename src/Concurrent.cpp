#include <Concurrent/Concurrent.h>

#include <thread>

namespace Concurrent
{
	unsigned int hardwareConcurrency()
	{
		return std::thread::hardware_concurrency();
	}
}