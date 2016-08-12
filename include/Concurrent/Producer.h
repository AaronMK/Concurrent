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

		Producer(Producer&& other);
		Producer();

		/**
		 * @brief
		 *  Destruction of the produder with an automatic end() call.  Any unconsumed items
		 *  in the queue upon destruction will be deleted.
		 */
		virtual ~Producer();

		/**
		 * @brief
		 *  Pushes a copy of the passed item into the message queue.
		 */
		bool push(const T &item);

		/**
		 * @brief
		 *  Pushes the passed item into the message queue using move semantics.
		 */
		bool push(T&& item);

		/**
		 * @brief
		 *  Takes an item out of the queue and places it in out.  This call will block
		 *  either until an item becomes available or until end is called.
		 *
		 * @return
		 *  True if an item was pulled from the internal queue and placed into out.
		 *  False if end() was called and there are no items in the queue.
		 */
		bool consume(T& out);

		/**
		 * @brief
		 *  Pulls an item from the queue and puts into out and returns true, or
		 *  returns false if there is none available.
		 */
		bool tryConsume(T& out);

		/**
		 * @brief
		 *  Returns true if the queue is empty.
		 */
		bool isEmpty() const;

		/**
		 * @brief
		 *  Marks the end of message production.
		 *
		 *  consume() calls will succeed until all items currently
		 *  in the queue have been consumed.  After that, consume() will
		 *  return false.  Any subsequent push() calls will fail.
		 */
		void end();

	private:
		Reference< ProducerInternal<T> > mInternal;
	};

	///////////// Implementation /////////////////

	template<typename T>
	Producer<T>::Producer(Producer<T>&& other)
		: mInternal(other.mInternal)
	{
		other.mInternal.makeNull();
	}

	template<typename T>
	Producer<T>::Producer()
	{
		mInternal = Reference< ProducerInternal<T> >::create();
		mInternal->endCalled.store(false);
	}

	template<typename T>
	Producer<T>::~Producer()
	{
		mInternal->end();
	}

	template<typename T>
	bool Producer<T>::push(const T &item)
	{
		if (mInternal->endCalled)
			return false;

		mInternal->pushMessage(item);
		return true;
	}

	template<typename T>
	bool Producer<T>::push(T&& item)
	{
		if (mInternal->endCalled)
			return false;

		mInternal->pushMessage(std::forward<T>(item));
		return true;
	}

	template<typename T>
	bool Producer<T>::consume(T& out)
	{
		Reference< ProducerInternal<T> > localInternal(mInternal);
		return localInternal->getMessage(out);
	}

	template<typename T>
	bool Producer<T>::tryConsume(T& out)
	{
		return mInternal->getMessage(out, true);
	}

	template<typename T>
	bool Producer<T>::isEmpty() const
	{
		return mInternal->messages.isEmpty();
	}

	template<typename T>
	void Producer<T>::end()
	{
		mInternal->end();
	}
}

#endif // _CONCURRENT_PRODUCE_CONSUME_H_
