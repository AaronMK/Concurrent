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
		QueuePlatform<std::optional<T>> mSysQueue;

		template<bool canCopy>
		void copyPush(const T& item);

		template<>
		void copyPush<true>(const T& item)
		{
			mSysQueue.push(std::make_optional<T>(item));
		}
		
		template<>
		void copyPush<false>(const T& item)
		{
			throw StdExt::invalid_operation("Attempting to copy a non-copyable object.")
		}
		
		template<bool canMove>
		void movePush(T&& item);

		template<>
		void movePush<true>(T&& item)
		{
			mSysQueue.push(std::make_optional<T>(std::move(item)));
		}
		
		template<>
		void movePush<false>(T&& item)
		{
			throw StdExt::invalid_operation("Attempting to move a non-movable object.")
		}

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
			copyPush<std::is_copy_constructible<T>::value>(item);
		}

		/**
		 * @brief
		 *  Pushes an item onto the Queue.
		 */
		void push(T&& item)
		{
			movePush<std::is_copy_constructible<T>::value>(std::move(item));
		}

		/**
		 * @brief
		 *  Attempts to pop an item from the Queue, if there is something to
		 *  de-queue, it is placed in destination and true is returned.
		 *  Otherwise, destination remains unchanged and false is returned.
		 */
		bool tryPop(T& destination)
		{
			std::optional<T> temp;
			if (mSysQueue.try_pop(temp))
			{
				destination = std::move(*temp);
				return true;
			}

			return false;
		}

		/**
		 * @brief
		 *  Attempts to pop an item from the Queue, if there is something to
		 *  de-queue, it is placed in destination and true is returned.
		 *  Otherwise, destination remains unchanged and false is returned.
		 */
		bool tryPop(std::optional<T>& destination)
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