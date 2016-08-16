#include <Concurrent/Condition.h>

#ifdef _WIN32

namespace Concurrent
{
	Condition::Condition()
	{
	}

	Condition::~Condition()
	{
		trigger();
	}

	bool Condition::wait()
	{
		return (0 == winEvent.wait(Concurrency::COOPERATIVE_TIMEOUT_INFINITE));
	}

	void Condition::trigger()
	{
		winEvent.set();
	}

	bool Condition::isTriggered() const
	{
		Condition* nonConstThis = const_cast<Condition*>(this);
		return (0 == nonConstThis->winEvent.wait(0));
	}

	void Condition::reset()
	{
		winEvent.reset();
	}
}

#endif