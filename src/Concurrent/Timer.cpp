#include <Concurrent/Timer.h>

#ifdef _WIN32

#include <cassert>
#include <type_traits>

using namespace Concurrency;
using namespace std;

namespace Concurrent
{
	TimerPlatform::TimerPlatform()
	{

	}

	TimerPlatform::~TimerPlatform()
	{

	}

	void TimerPlatform::constructTimer()
	{
		static_assert(std::is_same<interval_t, std::chrono::milliseconds>::value,
			"interval_t assumed to be milliseconds for current TimerPlatform::constructTimer() implementation.");

		assert(nullptr == mTimer);

		// Highly unlikely, but check anyway.
		assert(mInterval.count() <= std::numeric_limits<unsigned int>::max());

		static call<TimerPlatform*> funcCall([](TimerPlatform* t)
		{
			t->mHandler();
		});

		new (&mTimerMemory[0])sysTimer_t((unsigned int)mInterval.count(), this, &funcCall, mRepeat);
		mTimer = reinterpret_cast<sysTimer_t*>(&mTimerMemory[0]);
	}

	void TimerPlatform::clearTimer()
	{
		if (mTimer)
		{
			mTimer->stop();

			mTimer->~sysTimer_t();
			mTimer = nullptr;
		}
	}

	/////////////////////////////////////////

	Timer::Timer(std::function<void(void)>&& func, std::chrono::milliseconds interval)
		: Timer()
	{
		mTimer = nullptr;

		mInterval = interval;
		mHandler = std::forward<std::function<void(void)>>(func);
	}

	Timer::Timer()
	{
		mRepeat = false;
		mTimer = nullptr;
		mInterval = interval_t(0);
	}

	Timer::~Timer()
	{
		clearTimer();
	}

	void Timer::start(std::function<void(void)>&& func, interval_t interval)
	{
		clearTimer();

		mInterval = interval;
		mHandler = std::forward<std::function<void(void)>>(func);

		start();
	}

	void Timer::start()
	{
		if (mTimer && mRepeat)
		{
			mTimer->start();
		}
		else
		{
			clearTimer();

			mRepeat = true;

			constructTimer();
			mTimer->start();
		}
	}

	void Timer::oneShot(std::function<void(void)>&& func, interval_t interval)
	{
		clearTimer();

		mInterval = interval;
		mHandler = std::forward<std::function<void(void)>>(func);

		oneShot();
	}

	void Timer::oneShot()
	{
		if (mTimer && !mRepeat)
		{
			mTimer->start();
		}
		else
		{
			clearTimer();

			mRepeat = false;

			constructTimer();
			mTimer->start();
		}
	}

	void Timer::stop()
	{
		if (mTimer)
			mTimer->stop();
	}

	void Timer::clear()
	{
		clearTimer();

		mInterval = interval_t(0);
		mHandler = std::function<void()>();
	}
}

#endif // _WIN32