#ifndef _CONCURRENT_TASK_H_
#define _CONCURRENT_TASK_H_

#include "Config.h"

#include "Internal/TaskInternal.h"

namespace Concurrent
{
	/**
	 * @brief
	 *  Abstract task for concurrent operations.
	 * 
	 *  Tasks are the basic unit of work items that are passed into the system threadpool
	 *  via a Scheduler for execution.  They can be tracked, and subtasks for seperate
	 *  concurrent execution can be added as child tasks from within an implementation
	 *  of main(); 
	 */
	class CONCURRENT_EXPORT Task : public TaskInternal
	{
		friend class SchedulerInternal;
	public:
		Task(const Task&) = delete;
		Task& operator=(const Task&) = delete;

		Task();

		virtual ~Task();
		
		/**
		 * @brief
		 *  Override to determine what your task will do.
		 */
		virtual void main() = 0;

		/**
		 * @brief
		 *  The task is either currently running, or is in the queue waiting to run.
		 */
		bool isRunning();

		/**
		 * @brief
		 *  Waits for the task and its subtasks to complete.  This will return immediately if the
		 *  task has not been passed to the scheduler.
		 */
		void wait();

		/**
		 * @brief
		 *  Returns true if the task is running on the current thread.
		 */
		bool isCurrent() const;

		/**
		 * @brief
		 *  Gets a pointer to the Task that is currently running.  This will be nullptr if execution
		 *  is outside the context of a Scheduler.
		 */
		static Task* current();

		/**
		 * @brief
		 *  Waits for any task to complete, returning the index of the task that completed.
		 */
		static size_t waitForAny(Task** tArray, size_t numTasks);

		/**
		 * @brief
		 *  Waits for all tasks to complete.
		 */
		static void waitForAll(Task** tArray, size_t numTasks);

	protected:

		/**
		 * @brief
		 *  Adds the function as a subtask of this task.
		 *  
		 *  There is no tracking for the status provided by the tasking system, 
		 *  but the task calling this will not be signaled as complete until
		 *  func has also completed.
		 */
		bool subTask(std::function<void()>&& func);

		/**
		 * @brief
		 *  Adds the function as a subtask of this task, and runs it as a thread.
		 *
		 *  There is no tracking for the status provided by the tasking system,
		 *  but the task calling this will not be signaled as complete until
		 *  func has also completed.
		 */
		bool subTaskThread(std::function<void()>&& func);

		/**
		 * @brief
		 *  Adds a sub-task.
		 * 
		 *  This task will continue to run to completion, but will not
		 *  be considered complete, until decendent tasks are complete. The Task
		 *  must be running to create subtasks, and any subtask cannot be running
		 *  when attached. The subtask will be considered running upon return.
		 *
		 *  Subtasks will be placed on the highest priority queue.  This prevents a subtask from
		 *  effectively lowering the priority of the original task.
		 */
		bool subTask(Task* childTask);

		/**
		 * @brief
		 *  Adds a sub-task that is run as a thread.
		 *
		 *  This task will continue to run to completion, but will not be considered complete 
		 *  until decendent tasks are complete. The Task must be running to create subtasks,
		 *  and any subtask cannot be running when attached. The subtask will be considered
		 *  running upon return.
		 */
		bool subTaskThread(Task* childTask);

		/**
		 * @brief
		 *  Yields the current thread to work on another task.
		 *
		 *  This is good for keeping cores busy while waiting on non-cooperative
		 *  sychronization primitives.
		 */
		void yield();
	};
}

#endif // _CONCURRENT_TASK_H_