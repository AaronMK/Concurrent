#ifndef _CONCURRENT_CONDITION_PLATFORM_H_
#define _CONCURRENT_CONDITION_PLATFORM_H_

#include "../Config.h"

#ifdef _WIN32

#include <concrt.h>

namespace Concurrent
{
	/**
	 * @internal
	 */
	class CONCURRENT_EXPORT ConditionPlatform
	{
	public:
		Concurrency::event winEvent;
	};
}
#else
#	error "Concurrent::Condition is not supported on this platform."
#endif

#endif // _CONCURRENT_CONDITION_PLATFORM_H_