#include <Concurrent/Scheduler.h>

#include <Concurrent/Task.h>
#include <Concurrent/ReadLocker.h>
#include <Concurrent/WriteLocker.h>
#include <Concurrent/ThreadLocal.h>

#include "private_include/Platform.h"

#include <thread>
#include <algorithm>

namespace Concurrent
{
	static void ScheduleTaskRunner(Scheduler* scheduler)
	{
		sysScheduleFunction(&Concurrent::SchedulerInternal::taskRunner, scheduler);
	}

	template <typename T>
	static T clamp(T val, T min, T max)
	{
		return std::max<T>(min, std::min<T>(val, max));
	}

	////////////////////////////////////////////////////////////

	static Scheduler defaultScheduler(0);

	Scheduler::Scheduler(int maxPriority)
	{
		if (maxPriority < 0)
			maxPriority = 0;

		mInternal = Reference<SchedulerInternal>::create(maxPriority);
	}

	Scheduler::Scheduler(Scheduler&& other)
	{
		mInternal = other.mInternal;
	}

	Scheduler::~Scheduler()
	{
	}

	void Scheduler::addTask(std::function<void()>&& func, int priority)
	{	
		SchedulerInternal::TaskRecord record;
		record.func = std::forward<std::function<void()>>(func);
		record.ref = mInternal;

		mInternal->enqueueRecord(std::move(record), priority);
	}

	void Scheduler::addTask(Task* task, int priority)
	{
		task->schedulerAcquire();
		task->mScheduler = this;

		addTask(std::bind(&Task::doRun, task), priority);
	}

	Scheduler& Scheduler::operator=(Scheduler&& other)
	{
		mInternal = other.mInternal;
		return *this;
	}

	Scheduler* Scheduler::default()
	{
		return &defaultScheduler;
	}

	void Scheduler::runAsThread(Task* task)
	{
		task->schedulerAcquire();
		sysRunAsThread(std::bind(&SchedulerInternal::threadRunner, task));
	}

	void Scheduler::runInline(Task* task)
	{
		task->schedulerAcquire();

		task->doRun();
		task->wait();
	}

	void Scheduler::runAsync(Task* task)
	{
		default()->addTask(task, 0);
	}

	void Scheduler::runAsync(std::function<void()>&& func)
	{
		default()->addTask(std::move(func), 0);
	}

	/////////////////////////////////////////////////

	SchedulerInternal::SchedulerInternal(size_t maxPriority)
	{
		mTaskQueues.resize(maxPriority + 1);
	}
	
	SchedulerInternal::~SchedulerInternal()
	{
	}

	void SchedulerInternal::enqueueRecord(TaskRecord&& record, int priority)
	{
		assert(record.ref.get() == this);

		if (priority < 0)
		{
			highPriorityQueue.push(std::move(record));
		}
		else
		{
			priority = clamp<int>(priority, 0, (int)mTaskQueues.size() - 1);
			mTaskQueues[priority].push(std::move(record));
		}

		sysScheduleFunction(&SchedulerInternal::taskRunner, this);
	}

	void SchedulerInternal::taskRunner(void* data)
	{
		SchedulerInternal* schedulerInternal = static_cast<SchedulerInternal*>(data);

		TaskRecord record;
		if (false == schedulerInternal->highPriorityQueue.tryPop(record))
		{
			for (size_t i = schedulerInternal->mTaskQueues.size() - 1;
			     i >= 0 && false == schedulerInternal->mTaskQueues[i].tryPop(record); i--)
			{
			}
		}

		record.func();

		if (record.parentTask)
			record.parentTask->schedulerRelease();
	}

	void SchedulerInternal::threadRunner(Task* task)
	{
		task->doRun();
		task->wait();
	}
}