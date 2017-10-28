#ifndef _CONCURRENT_OBJECT_POOL_INTERNAL_H_
#define _CONCURRENT_OBJECT_POOL_INTERNAL_H_

#include "Concurrent.h"
#include "Producer.h"

#include <functional>

namespace Concurrent
{
	template<typename T>
	class PoolObject;

	template<typename T>
	class ObjectPool;

	/**
	 * @internal
	 *
	 * @brief
	 *  Internal %ObjectPool data and structures that has shared ownership
	 *  with an %ObjectPool and dependent PoolObjects.
	 */
	template<typename T>
	class ObjectPoolInternal
	{
		friend PoolObject<T>;
		friend ObjectPool<T>;

	private:

		/**
		 * @brief
		 *  The items in the pool that are not out in circulation.
		 */
		Producer<T> mItemPool;

		/**
		 * @brief
		 *  Function used to construct a new item for the object pool when needed.
		 */
		std::function<T()> mConstructor;

		/**
		 * @brief
		 *  Function to reinitialize an item when it is returned to the pool.
		 */
		std::function<void(T&)> mReinit;

		/**
		 * @brief
		 *  The maximum number of items that can be made available by this %ObjectPool.
		 */
		size_t mMaxSize;

		/**
		 * @brief
		 *  The total number of items, including those in cirulation and those inside the pool.
		 */
		std::atomic<int32_t> mCirculatingItems;

		/**
		 * @brief
		 *  Attempts to get an item out of the pool.  If the pool is empty and the limit of circulating
		 *  items has not been reached, a new item will be constructed.  Otherwise, the function will
		 *  block until an item is returned.
		 *  
		 * @param out
		 *  Container into which the pool item will be placed.
		 *  
		 * @return
		 *  True if successful, false if there is an error.
		 */
		bool getItem(std::optional<T>& out)
		{
			if (false == mItemPool.tryConsume(out))
			{
				if (++mCirculatingItems > mMaxSize)
				{
					--mCirculatingItems;
					return mItemPool.consume(out);
				}
				else
				{
					// mCirculatingItems has already been incremented
					// to account for this.
					out.emplace(mConstructor());
					return true;
				}
			}

			return true;
		}

		/**
		 * @brief
		 *  Retinitializes <i>in</i> and moves it back into the pool.
		 */
		void returnItem(T&& in)
		{
			if (mCirculatingItems < mMaxSize)
			{
				mReinit(in);
				mItemPool.push(std::move(in));
			}
			else
			{
				--mCirculatingItems;
			}
		}
	};
}


#endif // _CONCURRENT_OBJECT_POOL_INTERNAL_H_