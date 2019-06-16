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
		std::function<void(const msg_t&)> mHandler;
		FunctionTask mLoopTask;
		Condition mHandleMessages;

		std::atomic<bool> mContinue;

		Queue<msg_t> mQueue;

		void loop()
		{
			while(!mQueue.isEmpty() || mContinue)
			{
				mHandleMessages.wait();
				mHandleMessages.reset();

				msg_t currentMessage;
				while (mQueue.tryPop(currentMessage))
					mHandler(currentMessage);
			}
		}

		void startTask(bool runAsThread)
		{
			mLoopTask.setFunction(std::bind(&MessageLoop::loop, this));

			if (runAsThread)
				Scheduler::runAsThread(&mLoopTask);
			else
				Scheduler::runAsync(&mLoopTask);
		}

	public:
		MessageLoop(const std::function<void(const msg_t&)>& msgHandler, bool runAsThread = false)
			: mHandler(msgHandler), mContinue(true)
		{
			startTask(runAsThread);
		}

		MessageLoop(std::function<void(const msg_t&)>&& msgHandler, bool runAsThread = false)
			: mHandler(std::move(msgHandler)), mContinue(true)
		{
			startTask(runAsThread);
		}

		~MessageLoop()
		{
			mContinue.store(false);
			mHandleMessages.trigger();

			mLoopTask.wait();
		}

		void push(const msg_t& msg)
		{
			mQueue.push(msg);
			mHandleMessages.trigger();
		}

		void push(msg_t&& msg)
		{
			mQueue.push(std::move(msg));
			mHandleMessages.trigger();
		}

		template<size_t size>
		void push(const std::array<msg_t, size>& list)
		{
			for(const msg_t& msg : list)
				mQueue.push(msg);

			mHandleMessages.trigger();
		}
		
		template<size_t size>
		void push(std::array<msg_t, size>&& list)
		{
			for(size_t i = 0; i < list.size(); ++i)
				mQueue.push(std::move(list[i]));

			mHandleMessages.trigger();
		}

		void push(const std::initializer_list<msg_t>& list)
		{
			for(const msg_t& msg : list)
				mQueue.push(msg);

			mHandleMessages.trigger();
		}

		void push(const std::vector<msg_t> list)
		{
			for(const msg_t& msg : list)
				mQueue.push(msg);

			mHandleMessages.trigger();
		}

		void push(std::vector<msg_t>&& list)
		{
			for(size_t i = 0; i < list.size(); ++i)
				mQueue.push(std::move(list[i]));

			mHandleMessages.trigger();
			list.clear();
		}
	};
}

#endif // _CONCURRENT_MESSAGE_LOOP_H_