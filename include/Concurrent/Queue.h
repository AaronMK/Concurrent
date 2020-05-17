#ifndef _CONCURRENT_QUEUE_H_
#define _CONCURRENT_QUEUE_H_

#include "Config.h"
#include "Internal/QueuePlatform.h"

#include <StdExt/Exceptions.h>
#include <StdExt/Type.h>

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
		static_assert(
			!(
				StdExt::Traits<T>::is_reference || StdExt::Traits<T>::is_move_reference ||
				(StdExt::Traits<T>::is_const && !StdExt::Traits<T>::is_pointer)
			),
			"Reference or constant types are not allowed as a Queue type."
		);

		static constexpr bool use_actual_type = StdExt::Traits<T>::default_constructable;
		using queue_t = std::conditional_t<use_actual_type, T, std::optional<T>>;

	private:
		QueuePlatform<queue_t> mSysQueue;

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
			if constexpr (use_actual_type)
				mSysQueue.push(item);
			else
				mSysQueue.push(std::make_optional<T>(item));
		}

		/**
		 * @brief
		 *  Pushes an item onto the Queue.
		 */
		void push(T&& item)
		{
			if constexpr (use_actual_type)
				mSysQueue.push(std::move(item));
			else
				mSysQueue.push(std::make_optional<T>(std::move(item)));
		}

		/**
		 * @brief
		 *  Attempts to pop an item from the Queue, if there is something to
		 *  de-queue, the returned optional will contain the item.  Otherwise,
		 *  an empty optional will be returned.
		 */
		std::optional<T> pop()
		{
			if constexpr (use_actual_type)
			{
				T ret;

				if (mSysQueue.try_pop(ret))
					return std::make_optional(std::move(ret));
				else
					return std::optional<T>();
			}
			else
			{
				std::optional<T> ret;

				mSysQueue.try_pop(ret);
				return ret;
			}
		}

		/**
		 * @brief
		 *  Attempts to pop an item from the Queue. If there is something to
		 *  de-queue, it is placed in destination and true is returned.
		 *  Otherwise, destination remains unchanged and false is returned.
		 */
		bool tryPop(T* destination)
		{
			if constexpr (use_actual_type)
			{
				return mSysQueue.try_pop(*destination);
			}
			else
			{
				std::optional<T> ret;

				if (mSysQueue.try_pop(ret))
				{
					*destination = ret.value();
					return true;
				}

				return false;
			}
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