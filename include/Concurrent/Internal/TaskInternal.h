#ifndef _CONCURRENT_TASK_INTERNAL_H_
#define _CONCURRENT_TASK_INTERNAL_H_

#include "../Condition.h"

#include "SchedulerInternal.h"

#include <atomic>

namespace Concurrent
{
	/**
	 * @internal
	 */
	class CONCURRENT_EXPORT TaskInternal
	{
		friend class Task;
		friend class Scheduler;
		friend class SchedulerInternal;

	private:
		static size_t waitForMultiple(Task** tArray, size_t numTasks, bool all);

		TaskInternal();
		virtual ~TaskInternal();

		void schedulerAcquire();
		void schedulerRelease();

		std::atomic<int> mDependentCount;
		Condition mFinishedHandle;

		Scheduler* mScheduler;
		Task* mParent;

		void doRun();
	};
}

#endif // _CONCURRENT_TASK_INTERNAL_H_