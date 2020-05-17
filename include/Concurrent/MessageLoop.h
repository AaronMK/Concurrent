#ifndef _CONCURRENT_MESSAGE_LOOP_H_
#define _CONCURRENT_MESSAGE_LOOP_H_

#include "Queue.h"
#include "Mutex.h"
#include "Scheduler.h"
#include "Condition.h"
#include "MutexLocker.h"
#include "FunctionTask.h"


#include <array>
#include <atomic>
#include <thread>
#include <vector>
#include <iterator>
#include <functional>
#include <initializer_list>

#include <StdExt/Type.h>

namespace Concurrent
{
	template<typename msg_t>
	class MessageLoop
	{
		static_assert(
			!(
				StdExt::Traits<msg_t>::is_reference || StdExt::Traits<msg_t>::is_move_reference ||
				(StdExt::Traits<msg_t>::is_const && !StdExt::Traits<msg_t>::is_pointer)
			),
			"Reference or constant types are not allowed as a Queue type."
		);

	private:
		FunctionTask mLoopTask;
		Condition mHandleMessages;
		std::atomic<bool> mContinue;

		using traits_t = StdExt::Traits<msg_t>;
		using varient_t = std::variant<msg_t, Condition*>;

		Queue<varient_t> mQueue;

		void loop()
		{
			initialize();

			while(!mQueue.isEmpty() || mContinue)
			{
				mHandleMessages.wait();
				mHandleMessages.reset();

				varient_t currentMessage;
				while (mQueue.tryPop(&currentMessage))
				{
					if (std::holds_alternative<msg_t>(currentMessage))
						handleMessage(std::get<msg_t>(currentMessage));
					else
						std::get<Condition*>(currentMessage)->trigger();
				}
			}

			finalize();
		}

		void startTask(bool runAsThread)
		{
			mLoopTask.setFunction(std::bind(&MessageLoop::loop, this));
			mContinue.store(true);

			if (runAsThread)
				Scheduler::runAsThread(&mLoopTask);
			else
				Scheduler::runAsync(&mLoopTask);
		}

	protected:

		using handle_message_t = typename traits_t::arg_non_copy_t;
		
		/**
		 * @brief
		 *  Method called to initialize the message loop on each start within the context
		 *  of the loop.
		 */
		virtual void initialize()
		{
		}

		/**
		 * @brief
		 *  Method called to finalize the message loop after stop is called and the
		 *  last message is processed.
		 */
		virtual void finalize()
		{
		}

		virtual void handleMessage(handle_message_t msg) = 0;

	public:

		MessageLoop()
		{
		}

		~MessageLoop()
		{
			stop();
		}

		void start(bool runAsThread = false)
		{
			if (mLoopTask.isRunning())
				return;

			startTask(runAsThread);
		}

		void stop()
		{
			if (mLoopTask.isRunning())
			{
				mContinue.store(false);
				mHandleMessages.trigger();

				mLoopTask.wait();
			}
		}

		/**
		 * @brief
		 *  Inserts the passed condition into the message loop and triggers
		 *  it when it is reached.
		 */
		void fence(Condition* signal)
		{
			signal->reset();
			mQueue.push(signal);

			mHandleMessages.trigger();
		}

		/**
		 * @brief
		 *  Blocks until all messages in the loop at the time of this call
		 *  have been processed.
		 */
		void fence()
		{
			Condition signal;

			mQueue.push(varient_t(&signal));
			mHandleMessages.trigger();

			signal.wait();
		}

		/**
		 * @brief
		 *  Pushes an item onto the Queue.
		 */
		void push(const msg_t& item)
		{
			mQueue.push(varient_t(item));
			mHandleMessages.trigger();
		}

		/**
		 * @brief
		 *  Pushes an item onto the Queue.
		 */
		void push(msg_t&& item)
		{
			mQueue.push(varient_t(std::move(item)));
			mHandleMessages.trigger();
		}

		template<typename iterator>
		void push_items(iterator begin, iterator end, Condition* signal = nullptr)
		{
			for (iterator itr = begin; itr != end; ++itr)
				mQueue.push(*itr);

			if (signal)
				mQueue.push(signal);

			mHandleMessages.trigger();
		}
	};
}

#endif // _CONCURRENT_MESSAGE_LOOP_H_