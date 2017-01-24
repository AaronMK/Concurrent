#ifndef _CONCURRENT_FUNCTION_TASK_H_
#define _CONCURRENT_FUNCTION_TASK_H_

#include "Task.h"

#include <functional>

namespace Concurrent
{
	/**
	 * @brief
	 *  A convenience class that that makes it easy to schedule and run a
	 *  function as a trackable Task.
	 */
	class CONCURRENT_EXPORT FunctionTask : public Task
	{
	public:
		FunctionTask() = default;

		/**
		 * @brief
		 *  Creates a task that will run the passed function when scheduled.
		 */
		FunctionTask(std::function<void()>&& func);

		virtual ~FunctionTask();

		/**
		 * @brief
		 *  Sets the function that will run when the task is scheduled.  It is an error
		 *  to call this function when the task is in the running state.
		 */
		void setFunction(std::function<void()>&& func);

		/**
		 * @brief
		 *  Overriden to call the function wrapped in the task.
		 */
		virtual void run() override;

	private:
		std::function<void()> mFunc;
	};
}

#endif // _CONCURRENT_FUNCTION_TASK_H_