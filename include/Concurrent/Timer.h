#ifndef _CONCURRENT_TIMER_H_
#define _CONCURRENT_TIMER_H_

#include "Internal/TimerPlatform.h"

#include <functional>
#include <chrono>

namespace Concurrent
{
	/**
	 * @brief
	 *  A timer object that will call a passed function at set intervals or after a delay.  Intervals
	 *  will be as accurate as the underlying concurrency framework allows.  TimerPlatform::interval_t
	 *  is defined from the std::chrono namespace to reflect underlying precision. 
	 */
	class CONCURRENT_DYNAMIC_CLASS Timer : public TimerPlatform
	{
	public:
		Timer(const Timer&) = delete;
		Timer(Timer&&) = delete;

		/**
		 * @brief
		 *  Constructs a timer with the passed callback and interval.  start() or oneShot()
		 *  must be called to active it.
		 */
		Timer(std::function<void(void)>&& func, interval_t interval);
		
		/**
		 * @brief
		 *  Constructs an emprty timer.
		 */
		Timer();

		/**
		 * @brief
		 *  Destruction of the timer, stopping it if it is running.
		 */
		virtual ~Timer();

		/**
		 * @brief
		 *  Configures and starts the timer.
		 */
		void start(std::function<void(void)>&& func, interval_t interval);

		/**
		 * @brief
		 *  Starts an already configured timer.
		 */
		void start();
		
		/**
		 * @brief
		 *  Calls the passed function after the passed number of milliseconds.
		 *  
		 *  The timer will remain configured with the passed parameters.
		 */
		void oneShot(std::function<void(void)>&& func, interval_t interval);

		/**
		 * @brief
		 *  Triggers the timer once with parameters already passed in set functions.
		 */
		void oneShot();

		/**
		 * @brief
		 *  Stops the timer.
		 */
		void stop();

		/**
		 * @brief
		 *  Clear callback and timing settings.
		 */
		void clear();
	};
}

#endif // _CONCURRENT_TIMER_H_