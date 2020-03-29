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
#include <functional>
#include <initializer_list>

namespace Concurrent
{
	template<typename msg_t>
	class MessageLoop
	{
	private:
		FunctionTask mLoopTask;
		Condition mHandleMessages;

		std::atomic<bool> mContinue;

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
				while (mQueue.tryPop(currentMessage))
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
		
		/**
		 * @brief
		 *  Method called to initialize the message loop on each start within the context
		 *  of the loop.
		 */
		virtual void initialize()
		{
		}

		virtual void finalize()
		{
		}

		virtual void handleMessage(msg_t msg) = 0;

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
			mQueue.push(varient_t(signal));
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
			signal.wait();
		}

		void push(const msg_t& msg, Condition* signal = nullptr)
		{
			mQueue.push(msg);

			if (signal)
				fence(signal);

			mHandleMessages.trigger();
		}

		void push(msg_t&& msg, Condition* signal = nullptr)
		{
			mQueue.push(std::move(msg));

			if (signal)
				fence(signal);

			mHandleMessages.trigger();
		}

		template<size_t size>
		void push(const std::array<msg_t, size>& list, Condition* signal = nullptr)
		{
			for(const msg_t& msg : list)
				mQueue.push(msg);

			if (signal)
				fence(signal);

			mHandleMessages.trigger();
		}
		
		template<size_t size>
		void push(std::array<msg_t, size>&& list, Condition* signal = nullptr)
		{
			for(size_t i = 0; i < list.size(); ++i)
				mQueue.push(std::move(list[i]));

			mHandleMessages.trigger();
		}

		void push(const std::initializer_list<msg_t>& list, Condition* signal = nullptr)
		{
			for(const msg_t& msg : list)
				mQueue.push(msg);

			if (signal)
				fence(signal);

			mHandleMessages.trigger();
		}

		void push(const std::vector<msg_t> list, Condition* signal = nullptr)
		{
			for(const msg_t& msg : list)
				mQueue.push(msg);

			if (signal)
				fence(signal);

			mHandleMessages.trigger();
		}

		void push(std::vector<msg_t>&& list, Condition* signal = nullptr)
		{
			for(size_t i = 0; i < list.size(); ++i)
				mQueue.push(std::move(list[i]));

			if (signal)
				fence(signal);

			mHandleMessages.trigger();
			list.clear();
		}
	};
}

#endif // _CONCURRENT_MESSAGE_LOOP_H_