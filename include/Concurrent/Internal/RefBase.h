#ifndef _CONCURRENT_REF_BASE_H_
#define _CONCURRENT_REF_BASE_H_

#include "../Config.h"

#include <memory>

namespace Concurrent
{	
	/**
	 * @internal
	 */
	template<typename T>
	class RefBase
	{
	protected:
		RefBase()
		{
		}

		RefBase(const std::shared_ptr<T> &ptr)
		{
			std::atomic_exchange<T>(&mPtr, ptr);
		}

		virtual ~RefBase()
		{
		}

		std::shared_ptr<T> mPtr;
	};

	template<typename T>
	class WeakRefBase
	{
	protected:
		WeakRefBase()
		{
		}

		WeakRefBase(const std::weak_ptr<T> &ptr)
			: mWeakPtr(ptr)
		{
		}

		WeakRefBase(const std::shared_ptr<T> &ptr)
			: mWeakPtr(ptr)
		{
		}

		virtual ~WeakRefBase()
		{
		}

		std::weak_ptr<T> mWeakPtr;
	};
}

#endif // _CONCURRENT_REF_BASE_H_