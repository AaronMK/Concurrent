#ifndef _CONCURRENT_THREAD_LOCAL_PTR_H_
#define _CONCURRENT_THREAD_LOCAL_PTR_H_

#include "Config.h"

namespace Concurrent
{
	/**
	 * @brief
	 *  A pointer that can be set independently for each thread or task.
	 *
	 *  This is a pointer that can be given a different value for each thread.  
	 *  This class does no allocation or deletion of the object to which it points, it simply 
	 *  stores the thread local value of the pointer.  This class should be used sparingly
	 *  since the number of thread local pointers that can be registered per process is limited
	 *  on some systems.
	 */
	template<typename T>
	class ThreadLocalPtr
	{
	public:

		/**
		 * @brief
		 *  Constructs a thread local pointer, registering it with the runtime.
		 */
		ThreadLocalPtr();

		/**
		 * @brief
		 *  Unregisters the thread local pointer from the runtime.
		 */
		virtual ~ThreadLocalPtr();

		/**
		 * @brief
		 *  "Dereference" operator acts as if the thread local poiter were a normal pointer.
		 */
		operator T*();

		/**
		 * @brief
		 *  "Member" operator acts as if the thread local poiter were a normal pointer.
		 */
		T* operator->();

		/**
		 * @brief
		 *  "Member" operator acts as if the thread local pointer were a normal pointer.
		 */
		const T* operator->() const;

		/**
		 * @brief
		 *  Obtains the thread's value of the pointer.
		 */
		const T* get() const;

		/**
		 * @brief
		 *  Obtains the thread's value of the pointer.
		 */
		T* get();

		/**
		 * @brief
		 *  Sets the value of the pointer for the current thread.
		 */
		void set(T* val);

	private:
#		if defined(_WIN32)
		DWORD key;
#		elif defined(__GNUC__)
		pthread_key_t key;
#		endif
	};

	template<typename T>
	class ThreadLocal
	{
	private:
		ThreadLocalPtr<T> mPtr;
		T mDefaultValue;

		T* fetchPtr()
		{
			T* ret = mPtr.get();

			if (nullptr == ret)
			{
				mPtr.set(new T(mDefaultValue));
				return mPtr.get();
			}

			return ret;
		}

	public:
		ThreadLocal()
		{
		}

		ThreadLocal(const T& defaultValue)
			: mDefaultValue(defaultValue)
		{
		}

		ThreadLocal(T&& defaultValue)
			: mDefaultValue(std::move(defaultValue))
		{
		}

		T* get()
		{
			return fetchPtr();
		}

		const T* get() const
		{
			return const_cast<const T*>(fetchPtr());
		}

		T* operator->()
		{
			return get();
		}

		const T* operator->() const
		{
			return get();
		}

		T& operator*()
		{
			return *get();
		}

		const T& operator*() const
		{
			return *get();
		}
	};

#if defined(_WIN32)
	template<typename T>
	ThreadLocalPtr<T>::ThreadLocalPtr()
	{
		key = TlsAlloc();
		TlsSetValue(key, nullptr);
	}

	template<typename T>
	ThreadLocalPtr<T>::~ThreadLocalPtr()
	{
		TlsFree(key);
	}

	template<typename T>
	ThreadLocalPtr<T>::operator T*()
	{
		return static_cast<T*>(TlsGetValue(key));
	}

	template<typename T>
	T* ThreadLocalPtr<T>::operator->()
	{
		return static_cast<T*>(TlsGetValue(key));
	}

	template<typename T>
	const T* ThreadLocalPtr<T>::operator->() const
	{
		return static_cast<const T*>(TlsGetValue(key));
	}

	template<typename T>
	const T* ThreadLocalPtr<T>::get() const
	{
		return static_cast<const T*>(TlsGetValue(key));
	}

	template<typename T>
	T* ThreadLocalPtr<T>::get()
	{
		return static_cast<T*>(TlsGetValue(key));
	}

	template<typename T>
	void ThreadLocalPtr<T>::set(T* val)
	{
		TlsSetValue(key, val);
	}
#elif defined(__GNUC__)
	template<typename T>
	ThreadLocalPtr<T>::ThreadLocalPtr()
	{
		pthread_key_create(&key, nullptr);
		pthread_setspecific(key, nullptr);
	}

	template<typename T>
	ThreadLocalPtr<T>::~ThreadLocalPtr()
	{
		pthread_key_delete(key);
	}

	template<typename T>
	T& ThreadLocalPtr<T>::operator *()
	{
		return static_cast<T*>(pthread_getspecific(key));
	}

	template<typename T>
	T* ThreadLocalPtr<T>::operator ->()
	{
		return static_cast<T*>(pthread_getspecific(key));
	}

	template<typename T>
	ThreadLocalPtr<T>& ThreadLocalPtr<T>::operator=(T* val)
	{
		pthread_setspecific(key, val);
		return *this;
	}
#else
#	error Need to implement ThreadLocalPtr on current platform.
#endif
}

#endif // _CONCURRENT_THREAD_LOCAL_PTR_H_