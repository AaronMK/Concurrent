#ifndef _SYS_COMM_OBJECT_POOL_H_
#define _SYS_COMM_OBJECT_POOL_H_


#include "Internal/ObjectPoolInternal.h"

#include "Concurrent.h"

#include <atomic>
#include <functional>
#include <type_traits>

namespace Concurrent
{
	/**
	 * @brief
	 *  A implementation of the Object Pool pattern.
	 *
	 * @todo
	 *  Make this more easily support the not having to supply a constructor paramater
	 *  if the default constructor of T is desired.
	 */
	template<typename T>
	class ObjectPool
	{
		friend class PoolObject<T>;

		static_assert(std::is_move_constructible<T>::value, "ObjectPool type must be move constructible.");
	
	private:

	public:
		ObjectPool(const ObjectPool&) = delete;
		ObjectPool(ObjectPool&&) = delete;

		typedef std::function<void(T&)> reintFunc;
		typedef std::function<T()> constructFunc;

		/**
		 * @brief
		 *  Creates an uninitialized %ObjectPool. init() must be called before use.
		 */
		ObjectPool()
		{
		}

		virtual ~ObjectPool()
		{
		}
		
		template<typename type = T>
		void init(std::enable_if_t<std::is_default_constructible_v<type>, size_t> maxSize = 0)
		{
			init([]()
			{ 
				return type();
			}, maxSize);
		}

		/**
		 * @brief
		 *  Initialization of the object pool that does not set any reinitialization of
		 *  objects when they are returned to the pool.
		 *
		 * @param constructor
		 *  A function that is used to construct more items for the %MemoryPool when needed.
		 *  
		 * @param maxSize
		 *  The maximum number of objects in circulation.  This includes items in the pool
		 *  as well as as those out in PoolObjects.  Items will be created as needed until
		 *  the maximum size is hit.  After that, items will be given as they are returned
		 *  to the pool.  If 0, the max size will be hardwareConcurrency().
		 */
		void init(constructFunc&& constructor, size_t maxSize = 0)
		{
			auto reInit = [](T&)
			{
			};
		
			init(std::move(constructor), std::move(reInit), maxSize);
		}

		/**
		 * @brief
		 *  Initialization of the object pool uses the default constructor to create new
		 *  objects and the passed function for reinitialization of returned items.
		 *
		 * @param reInit
		 *  A function that is used to re-initialize objects when they are returned to the pool.
		 *  
		 * @param maxSize
		 *  The maximum number of objects in circulation.  This includes items in the pool
		 *  as well as as those out in PoolObjects.  Items will be created as needed until
		 *  the maximum size is hit.  After that, items will be given as they are returned
		 *  to the pool.  If 0, the max size will be hardwareConcurrency().
		 */
		template<typename type = T>
		void init(reintFunc&& reInit, std::enable_if_t<std::is_default_constructible_v<type>, size_t> maxSize = 0)
		{
			init(
				[]()
				{
					return T();
				},
				std::move(reInit),
				maxSize);
		}

		/**
		 * @brief
		 *  Full initialization for the object pool.
		 *
		 * @param constructor
		 *  A function that is used to construct more items for the %MemoryPool when needed.
		 *
		 * @param reInit
		 *  A function that is used to re-initialize objects when they are returned to the pool.
		 *  
		 * @param maxSize
		 *  The maximum number of objects in circulation.  This includes items in the pool
		 *  as well as as those out in PoolObjects.  Items will be created as needed until
		 *  the maximum size is hit.  After that, items will be given as they are returned
		 *  to the pool.  If 0, the max size will be hardwareConcurrency().
		 */
		void init(std::function<T()>&& constructor, std::function<void(T&)>&& reInit, size_t maxSize = 0)
		{
			assert( mInternal.isNull() ); // init() can only be called once.

			mInternal = Reference< ObjectPoolInternal<T> >::create();

			mInternal->mCirculatingItems = 0;
			mInternal->mMaxSize = (0 == maxSize) ? Concurrent::hardwareConcurrency() : maxSize;

			mInternal->mConstructor = std::move(constructor);
			mInternal->mReinit = std::move(reInit);
		}

	private:
		Reference< ObjectPoolInternal<T> > mInternal;
	};

	/**
	 * @brief
	 *  An RAII handle to an object from an ObjectPool.
	 *
	 *  Objects of this class should only exist for a short amount of time.  They
	 *  will aquire an object from an ObjectPool, blocking during construction
	 *  until one becomes available, returning the object to the pool on destruction.
	 */
	template<typename T>
	class PoolObject
	{
	public:
		PoolObject(ObjectPool<T>* pool)
			: mInternal(pool->mInternal)
		{
			assert(mInternal); // Init has not been called on pool.
			mInternal->getItem(mItem);
		};

		virtual ~PoolObject()
		{
			free();
		}

		T* operator->()
		{
			assert(mItem); // Fails if free() has been called.
			return mItem.operator->();
		}

		const T* operator->() const
		{
			assert(mItem); // Fails if free() has been called.
			return mItem.operator->();
		}

		T& operator*()
		{
			return *mItem;
		}

		const T& operator*() const
		{
			return *mItem;
		}

		/**
		 * @brief
		 *  Returns the contained object to the ObjectPool from which
		 *  it came.
		 */
		void free()
		{
			if (mItem)
				mInternal->returnItem(std::move(*mItem));
		}

	private:
		/**
		 * @brief
		 *	Optional so T does not have to be default constructable, and does
		 *  not have to have a seperate allocation on the heap outside of
		 *  Producer memory.
		 */
		std::optional<T> mItem;

		Reference< ObjectPoolInternal<T> > mInternal;
	};
}

#endif // _SYS_COMM_OBJECT_POOL_H_