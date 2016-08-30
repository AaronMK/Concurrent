#ifndef _CONCURRENT_H_
#define _CONCURRENT_H_

#include "Config.h"

/**
 * @brief
 *  Contains all classes and functionality of the Concurrent library.
 */
namespace Concurrent
{
	/**
	 * @brief
	 *  Returns the number of logical cores on the host system.
	 */
	CONCURRENT_EXPORT unsigned int hardwareConcurrency();
}

#endif // _CONCURRENT_H_