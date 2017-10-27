#ifndef _CONCURRENT_PRODUCE_CONSUME_H_
#define _CONCURRENT_PRODUCE_CONSUME_H_

#include "Internal/ProducerInternal.h"

#include "Reference.h"

namespace Concurrent
{
	/**
	 * @brief
	 *  A class that facilitates the producer/consumer model.
	 *
	 *  The Producer object accepts items that are "pushed" into it.  Once
	 *  an item is pushed it will either be passed to a single thread that
	 *  is waiting on a consume() call or will store the object in an internal
	 *  queue waiting for the next consume() call.
	 */
	template<typename T>
	class Producer
	{
	public:
		Producer(const Producer<T>&) = delete;

		Producer(Producer&& other)
			: mInternal(other.mInternal)
		{
			other.mInternal.makeNull();
		}

		Producer()
		{
			mInternal = Reference< ProducerInternal<T> >::create();
			mInternal->endCalled.store(false);
		}

		/**
		 * @brief
		 *  Destruction of the produder with an automatic end() call.  Any unconsumed items
		 *  in the queue upon destruction will be deleted.
		 */
		virtual ~Producer()
		{
			mInternal->end();
		}

		/**
		 * @brief
		 *  Pushes a copy of the passed item into the message queue.
		 */
		bool push(const T &item)
		{
			if (mInternal->endCalled)
				return false;

			mInternal->pushMessage(item);
			return true;
		}

		/**
		 * @brief
		 *  Pushes the passed item into the message queue using move semantics.
		 */
		bool push(T&& item)
		{
			if (mInternal->endCalled)
				return false;

			mInternal->pushMessage(std::forward<T>(item));
			return true;
		}

		/**
		 * @brief
		 *  Takes an item out of the queue and places it in out.  This call will block
		 *  either until an item becomes available or until end is called.
		 *
		 * @return
		 *  True if an item was pulled from the internal queue and placed into out.
		 *  False if end() was called and there are no items in the queue.
		 */
		bool consume(T& out)
		{
			Reference< ProducerInternal<T> > localInternal(mInternal);
			return localInternal->getMessage(out);
		}

		/**
		 * @brief
		 *  Takes an item out of the queue and places it in out.  This call will block
		 *  either until an item becomes available or until end is called.
		 *
		 * @return
		 *  True if an item was pulled from the internal queue and placed into out.
		 *  False if end() was called and there are no items in the queue.
		 */
		bool consume(std::optional<T>& out)
		{
			Reference< ProducerInternal<T> > localInternal(mInternal);
			return localInternal->getMessage(out);
		}

		/**
		 * @brief
		 *  Pulls an item from the queue and puts into out and returns true, or
		 *  returns false if there is none available.
		 */
		bool tryConsume(T& out)
		{
			return mInternal->getMessage(out, true);
		}

		/**
		 * @brief
		 *  Pulls an item from the queue and puts into out and returns true, or
		 *  returns false if there is none available.
		 */
		bool tryConsume(std::optional<T>& out)
		{
			return mInternal->getMessage(out, true);
		}

		/**
		 * @brief
		 *  Returns true if the queue is empty.
		 */
		bool isEmpty() const
		{
			return mInternal->messages.isEmpty();
		}

		/**
		 * @brief
		 *  Marks the end of message production.
		 *
		 *  consume() calls will succeed until all items currently
		 *  in the queue have been consumed.  After that, consume() will
		 *  return false.  Any subsequent push() calls will fail.
		 */
		void end()
		{
			mInternal->end();
		}

	private:
		Reference< ProducerInternal<T> > mInternal;
	};
}

#endif // _CONCURRENT_PRODUCE_CONSUME_H_
