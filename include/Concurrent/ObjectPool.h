#ifndef _SYS_COMM_OBJECT_POOL_H_
#define _SYS_COMM_OBJECT_POOL_H_

#include "Concurrent.h"
#include "Producer.h"
#include "Reference.h"

#include <atomic>
#include <functional>
#include <type_traits>

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
		bool getItem(T&& out);
		void returnItem(T&& in);

		Producer<T> mItemPool;

		std::function<T()> mConstructor;
		std::function<void(T&)> mReinit;

		size_t mMinSize;
		size_t mMaxSize;

		std::atomic<int32_t> mCirculatingItems;
	};

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

		static_assert(std::is_move_assignable<T>::value, "MemoryPool type must be move assignable.");

	public:
		ObjectPool(const ObjectPool&) = delete;
		ObjectPool(ObjectPool&&) = delete;

		/**
		 * @brief
		 *  Creates an uninitialized %ObjectPool. init() must be called before use.
		 */
		ObjectPool();

		virtual ~ObjectPool();
		
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
		void init(std::function<T()>&& constructor, size_t maxSize = 0);

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
		void init(std::function<T()>&& constructor, std::function<void(T&)>&& reInit, size_t maxSize = 0);

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
		PoolObject(ObjectPool<T>* pool);
		virtual ~PoolObject();

		T* operator->();
		const T* operator->() const;

		T& operator*();
		const T& operator*() const;

		/**
		 * @brief
		 *  Returns the contained object to the ObjectPool from which
		 *  it came.
		 */
		void free();

	private:
		
		/**
		 * @brief
		 *	Memory so T does not have to be default constructable, and does
		 *  not have to have a seperate allocation on the heap outside of
		 *  Producer memory.
		 */
		unsigned char mItemData[sizeof(T)];

		/**
		 * @note
		 *  &mItemData[0] could be cast to access the T object, but having this
		 *  set makes debugging easier. Considering the usage model for PoolObject
		 *  items is to use them temporarily on the stack, runtime cost is minimal.
		 *  
		 */
		T* mItemPtr;

		Reference< ObjectPoolInternal<T> > mInternal;
	};

	/////////////////////////////////
	
	template<typename T>
	bool ObjectPoolInternal<T>::getItem(T&& out)
	{
		assert(0 < mMinSize); // init() needs to be called.

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
				out = std::move(mConstructor());
				return true;
			}
		}

		return true;
	}

	template<typename T>
	void ObjectPoolInternal<T>::returnItem(T&& item)
	{
		if (mCirculatingItems < mMaxSize)
		{
			mReinit(item);
			mItemPool.push(std::move(item));
		}
		else
		{
			--mCirculatingItems;
		}
	}

	/////////////////////////////////

	template<typename T>
	ObjectPool<T>::ObjectPool()
	{
	}

	template<typename T>
	ObjectPool<T>::~ObjectPool()
	{
	}

	
	template<typename T>
	void ObjectPool<T>::init(std::function<T()>&& constructor, size_t maxSize)
	{
		auto reInit = [](T&)
		{
		};
		
		init(std::move(constructor), std::move(reInit), maxSize);
	}

	template<typename T>
	void ObjectPool<T>::init(std::function<T()>&& constructor, std::function<void(T&)>&& reInit, size_t maxSize)
	{
		assert( mInternal.isNull() ); // init() can only be called once.

		mInternal = Reference< ObjectPoolInternal<T> >::create();

		mInternal->mCirculatingItems = 0;
		mInternal->mMaxSize = (0 == maxSize) ? Concurrent::hardwareConcurrency() : maxSize;

		mInternal->mConstructor = std::move(constructor);
		mInternal->mReinit = std::move(reInit);
	}
	
	/////////////////////////////////

	template<typename T>
	PoolObject<T>::PoolObject(ObjectPool<T>* pool)
		: mInternal(pool->mInternal)
	{
		mItemPtr = reinterpret_cast<T*>(&mItemData[0])

		assert(mInternal); // Init has not been called on pool.
		mInternal->getItem(*mItemPtr);

	}

	template<typename T>
	PoolObject<T>::~PoolObject()
	{
		free();
	}
	
	template<typename T>
	void PoolObject<T>::free()
	{
		if (mItemPtr)
		{
			mInternal->returnItem(*mItemPtr);
			mItemPtr = nullptr;
		}
	}

	template<typename T>
	T* PoolObject<T>::operator->()
	{
		assert(mItemPtr); // Fails if free() has been called.
		return mItemPtr;
	}
	
	template<typename T>
	const T* PoolObject<T>::operator->() const
	{
		assert(mItemPtr); // Fails if free() has been called.
		return mItemPtr;
	}

	template<typename T>
	T& PoolObject<T>::operator*()
	{
		assert(mItemPtr); // Fails if free() has been called.
		return *mItemPtr;
	}

	template<typename T>
	const T& PoolObject<T>::operator*() const
	{
		assert(mItemPtr); // Fails if free() has been called.
		return *mItemPtr;
	}
}

#endif // _SYS_COMM_OBJECT_POOL_H_