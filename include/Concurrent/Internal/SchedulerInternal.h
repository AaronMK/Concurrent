#ifndef _CONCURRENT_SCHEDULER_INTERNAL_H_
#define _CONCURRENT_SCHEDULER_INTERNAL_H_

#include "../RWLock.h"
#include "../Queue.h"

#include <atomic>
#include <vector>
#include <functional>
#include <cassert>

namespace Concurrent
{
	class Task;

	/**
	 * @internal
	 */
	class SchedulerInternal
	{
		friend class Task;
		friend class Scheduler;
	public:

		struct TaskRecord
		{
			std::function<void()> func;
			Reference<SchedulerInternal> ref;
			Task* parentTask;

			TaskRecord()
			{
				parentTask = nullptr;
			}

			TaskRecord(const TaskRecord &other)
			{
				// This should never be called, but is here for compatibility with Queues.
				assert(false);
			}

			TaskRecord(TaskRecord&& other)
			{
				ref = other.ref;
				func = std::move(other.func);
				parentTask = other.parentTask;
			}

			TaskRecord& operator=(TaskRecord&& other)
			{
				ref = other.ref;
				func = std::move(other.func);
				parentTask = other.parentTask;

				return *this;
			}
		};

		SchedulerInternal(size_t maxPriority);
		virtual ~SchedulerInternal();

		void enqueueRecord(TaskRecord&& record, int priority);

		static void taskRunner(void* scheduler);

		static void threadRunner(Task* task);

		Queue<TaskRecord> highPriorityQueue;
		std::vector< Queue<TaskRecord> > mTaskQueues;
	};
}

#endif // _CONCURRENT_SCHEDULER_INTERNAL_H_