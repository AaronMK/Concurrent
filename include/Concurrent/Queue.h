#ifndef _CONCURRENT_QUEUE_H_
#define _CONCURRENT_QUEUE_H_

#include "Config.h"
#include "Internal/QueuePlatform.h"

#include <StdExt/Exceptions.h>
#include <optional>

namespace Concurrent
{
	/**
	 * @brief
	 *  Queue with a lock-free implementation on supported platforms.
	 */
	template<typename T>
	class Queue
	{
	private:
		QueuePlatform<T> mSysQueue;

	public:

		/**
		 * @brief
		 *  Creates an empty queue.
		 */
		Queue()
		{
		}

		virtual ~Queue()
		{
		}

		/**
		 * @brief
		 *  Pushes an item onto the Queue.
		 */
		void push(const T& item)
		{
			if constexpr (std::is_copy_constructible<T>::value)
				mSysQueue.push(item);
			else
				static_assert(false, "Attempting to copy an item that is not copy-constructable into a queue.");
		}

		/**
		 * @brief
		 *  Pushes an item onto the Queue.
		 */
		void push(T&& item)
		{
			if constexpr (std::is_move_constructible<T>::value)
				mSysQueue.push(std::move(item));
			else
				static_assert(false, "Attempting to move an item that is not move-constructable into a queue.");
		}

		/**
		 * @brief
		 *  Attempts to pop an item from the Queue, if there is something to
		 *  de-queue, it is placed in destination and true is returned.
		 *  Otherwise, destination remains unchanged and false is returned.
		 */
		bool tryPop(T& destination)
		{
			return mSysQueue.try_pop(destination);
		}

		/**
		 * @brief
		 *  Inspector to determine if the queue is empty.
		 */
		bool isEmpty() const
		{
			return mSysQueue.empty();
		}
	};
}

#endif _CONCURRENT_QUEUE_H_