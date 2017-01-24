#include <Concurrent/FunctionTask.h>

namespace Concurrent
{
	FunctionTask::FunctionTask(std::function<void()>&& func)
		: mFunc(std::forward<std::function<void()>>(func))
	{

	}

	FunctionTask::~FunctionTask()
	{
	}

	void FunctionTask::setFunction(std::function<void()>&& func)
	{
		assert(false == Task::isRunning());
		mFunc = std::forward<std::function<void()>>(func);
	}

	void FunctionTask::run()
	{
		mFunc();
	}
}