#ifndef _CONCURRENT_QUEUE_PLATFORM_H_
#define _CONCURRENT_QUEUE_PLATFORM_H_

#include <Concurrent/Config.h>

#ifdef _WIN32

#include <StdExt/Exceptions.h>

#include <concurrent_queue.h>
#include <optional>

namespace Concurrent
{
	template<typename T>
	class QueuePlatform
	{
	private:
		concurrency::concurrent_queue<T> mQueue;

	public:
		void push(const T& inItem)
		{
			mQueue.push(inItem);
		}
		
		void push(T&& inItem)
		{
			mQueue.push(std::move(inItem));
		}

		bool try_pop(T& outItem)
		{
			return mQueue.try_pop(outItem);
		}

		bool empty() const
		{
			return mQueue.empty();
		}
	};
}

#endif

#endif // _CONCURRENT_QUEUE_PLATFORM_H_