#ifndef _CONCURRENT_CONFIG_H_
#define _CONCURRENT_CONFIG_H_

/**
 * \def CONCURRENT_DYNAMIC_LIB
 * Define if being compiled as a dynamic library.
 */
// #define CONCURRENT_DYNAMIC_LIB

/**
 * \def CONCURRENT_COMPILE
 * This should be defined by compiler input when compiling the library.
 */

////////////////////////////////////////////////////////////////////////////
//////// Configuration End - Don't modify anything below this line. ////////
////////////////////////////////////////////////////////////////////////////

// Disable the warnings about clients using interfaces.  They will not be touching the variables
// to which they apply.
#if defined(_WIN32) && defined(CONCURRENT_DYNAMIC_LIB) 
#	pragma warning( disable : 4251)
#endif

#if !defined(CONCURRENT_DYNAMIC_LIB)
#	define CONCURRENT_EXPORT
#elif defined(_WIN32)
#	if defined(CONCURRENT_COMPILE)
#		define CONCURRENT_EXPORT __declspec(dllexport)
#	else
#		define CONCURRENT_EXPORT __declspec(dllimport)
#	endif
#endif

#ifdef _WIN32
#	define NOMINMAX
#	include <Windows.h>
#elif defined(__APPLE__)
#	include <malloc/malloc.h>
#	include <libkern/OSAtomic.h>
#endif

#endif // _CONCURRENT_CONFIG_H_