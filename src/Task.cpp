#include <Concurrent/Task.h>

#include <Concurrent/Scheduler.h>
#include <Concurrent/ThreadLocal.h>

#include "private_include/Platform.h"

#include <cassert>
#include <thread>

namespace Concurrent
{
	static ThreadLocalPtr<Task> runningTask;

	/////////////////////////////////////

	TaskInternal::TaskInternal()
	{
		mFinishedHandle.trigger();
		mDependentCount.store(0);

		mScheduler = nullptr;
		mParent = nullptr;
	}

	TaskInternal::~TaskInternal()
	{
		assert(nullptr == mScheduler && nullptr == mParent);
	}

	void TaskInternal::schedulerAcquire()
	{
		if (1 == ++mDependentCount)
			mFinishedHandle.reset();
	}

	void TaskInternal::schedulerRelease()
	{
		if (0 == --mDependentCount)
		{
			if (mParent)
			{
				mParent->schedulerRelease();
				mParent = nullptr;
			}

			mScheduler = nullptr;

			// This must be last since any task owning code could destroy
			// the task once wait() returns.
			mFinishedHandle.trigger();
		}
	}

	void TaskInternal::doRun()
	{
		Task* task = dynamic_cast<Task*>(this);

		runningTask.set(task);
		task->run();
		runningTask.set(nullptr);

		task->schedulerRelease();
	}

	
	size_t TaskInternal::waitForMultiple(Task** tArray, size_t numTasks, bool all)
	{
		Concurrency::event** winEvents = (Concurrency::event**)alloca(sizeof(Concurrency::event*)*numTasks);

		for(size_t i = 0; i < numTasks; i++)
			winEvents[i] = &(tArray[i]->mFinishedHandle.winEvent);

		return Concurrency::event::wait_for_multiple(winEvents, numTasks, all);
	}

	///////////////////////////////////////

	Task::Task()
	{
	}

	Task::~Task()
	{
	}

	bool Task::isRunning()
	{
		return !mFinishedHandle.isTriggered();
	}

	void Task::wait()
	{
		mFinishedHandle.wait();
	}

	bool Task::isCurrent() const
	{
		return (this == current());
	}

	Task* Task::current()
	{
		return runningTask.get();
	}

	size_t Task::waitForAny(Task** tArray, size_t numTasks)
	{
		return waitForMultiple(tArray, numTasks, false);
	}

	void Task::waitForAll(Task** tArray, size_t numTasks)
	{
		waitForMultiple(tArray, numTasks, true);
	}

	bool Task::subTask(std::function<void()>&& func)
	{
		assert(this == current());

		schedulerAcquire();

		SchedulerInternal::TaskRecord record;
		Scheduler* scheduler = (mScheduler) ? mScheduler : Scheduler::default();

		record.func = std::forward<std::function<void()>>(func);
		record.ref = scheduler->mInternal;
		record.parentTask = this;

		scheduler->mInternal->enqueueRecord(std::move(record), -1);

		return true;
	}

	bool Task::subTaskThread(std::function<void()>&& func)
	{
		assert(this == current());

		schedulerAcquire();

		std::function<void(std::function<void()>)> runFunc = [this](std::function<void()> funcArg)
		{
			funcArg();
			schedulerRelease();
		};

		sysRunAsThread(std::bind(runFunc, std::forward<std::function<void()>>(func)));

		return true;
	}

	bool Task::subTask(Task* childTask)
	{
		assert(this == current());

		schedulerAcquire();

		SchedulerInternal::TaskRecord record;
		Scheduler* scheduler = (mScheduler) ? mScheduler : Scheduler::default();
		
		childTask->mParent = this;
		childTask->mScheduler = scheduler;

		record.func = std::bind(&TaskInternal::doRun, childTask);
		record.ref = scheduler->mInternal;
		record.parentTask = nullptr;

		scheduler->mInternal->enqueueRecord(std::move(record), -1);

		return true;
	}

	bool Task::subTaskThread(Task* childTask)
	{
		schedulerAcquire();

		sysRunAsThread([this, childTask]() {
			childTask->run();
			schedulerRelease();
		});

		return true;
	}

#ifdef _WIN32
	void Task::yield()
	{
		Concurrency::Context::CurrentContext()->YieldExecution();
	}
#else
#	error Need to implement Task::yield() on current platform.
#endif

}
