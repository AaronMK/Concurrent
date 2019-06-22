#ifndef _CONCURRENT_TIMER_PLATFORM_H_
#define _CONCURRENT_TIMER_PLATFORM_H_

#include "../Config.h"

#ifdef _WIN32

#include <agents.h>

#include <chrono>
#include <optional>

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

		using sysTimer_t = Concurrency::timer<TimerPlatform*>;

		std::optional<sysTimer_t> mTimer;

		bool mRepeat;
		std::function<void(void)> mHandler;
		interval_t mInterval;

		void constructTimer();
		void clearTimer();
	};
}

#endif 

#endif // _WIN32