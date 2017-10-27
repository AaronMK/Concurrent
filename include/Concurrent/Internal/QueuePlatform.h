#ifndef _CONCURRENT_QUEUE_PLATFORM_H_
#define _CONCURRENT_QUEUE_PLATFORM_H_

#include <Concurrent/Config.h>

#ifdef _WIN32

#include <StdExt/Exceptions.h>

#include <concurrent_queue.h>
#include <optional>

namespace Concurrent
{
	template<typename T>
	class QueuePlatform
	{
	protected:
		class Container
		{
		private:
			std::optional<T> mItem;

			template<bool canCopy>
			void setItemCopy(std::enable_if_t<canCopy, const T&> item)
			{
				mItem.emplace(item);
			}
			
			template<bool canCopy>
			void setItemCopy(std::enable_if_t<!canCopy, const T&> item)
			{
				throw StdExt::invalid_operation("Attempting to copy a non-copyable object.");
			}
			
			template<bool canMove>
			void setItemMove(std::enable_if_t<canMove, T&&> item)
			{
				mItem.emplace(std::move(item));
			}
			
			template<bool canMove>
			void setItemMove(std::enable_if_t<!canMove, T&&> item)
			{
				throw StdExt::invalid_operation("Attempting to move a non-movable object.");
			}

		public:
			Container()
			{
			}

			Container(const Container& other)
			{
				throw StdExt::invalid_operation("Container should not be copied.");
			}
			
			Container(Container&& other)
			{
				mItem = std::move(other.mItem);
			}

			Container(const T& item)
			{
				setItemCopy<std::is_copy_constructible_v<T>>(item);
			}

			Container(T&& item)
			{
				setItemMove<std::is_move_constructible_v<T>>(std::move(item));
			}

			Container& operator=(const Container& other)
			{
				throw StdExt::invalid_operation("Container should not be copied.");
				return *this;
			}

			Container& operator=(Container&& other)
			{
				mItem = std::move(other.mItem);
				return *this;
			}

			std::optional<T>& Item()
			{
				return mItem;
			}
		};
		
		concurrency::concurrent_queue<Container> mQueue;

	public:
		void push(const T& inItem)
		{
			mQueue.push(Container(inItem));
		}
		
		void push(T&& inItem)
		{
			mQueue.push(Container(std::move(inItem)));
		}

		bool try_pop(T& outItem)
		{
			Container tempContainer;

			if (mQueue.try_pop(tempContainer))
			{
				outItem = std::move(*tempContainer.Item());
				return true;
			}

			return false;
		}

		bool try_pop(std::optional<T>& outItem)
		{
			Container tempContainer;

			if (mQueue.try_pop(tempContainer))
			{
				outItem = std::move(tempContainer.Item());
				return true;
			}

			return false;
		}
	};
}

#endif

#endif // _CONCURRENT_QUEUE_PLATFORM_H_