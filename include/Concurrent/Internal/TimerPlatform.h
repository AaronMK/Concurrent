#ifndef _CONCURRENT_TIMER_PLATFORM_H_
#define _CONCURRENT_TIMER_PLATFORM_H_

#include "../Config.h"

#ifdef _WIN32

#include <agents.h>
#include <chrono>

namespace Concurrent
{
	/**
	 * @internal
	 */
	class CONCURRENT_EXPORT TimerPlatform
	{
	public:
		typedef std::chrono::milliseconds interval_t;

	protected:
		TimerPlatform();
		virtual ~TimerPlatform();

		typedef Concurrency::timer<TimerPlatform*> sysTimer_t;
		static const size_t sysTimerSize = sizeof(sysTimer_t);

		sysTimer_t* mTimer;
		unsigned char mTimerMemory[sysTimerSize];

		bool mRepeat;
		std::function<void(void)> mHandler;
		interval_t mInterval;

		void constructTimer();
		void clearTimer();
	};
}

#endif 

#endif // _WIN32