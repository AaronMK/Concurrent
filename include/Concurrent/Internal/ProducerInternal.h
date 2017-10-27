#ifndef _CONCURRENT_PRODUCE_CONSUME_INTERNAL_H_
#define _CONCURRENT_PRODUCE_CONSUME_INTERNAL_H_

#include "../Config.h"
#include "../Queue.h"
#include "../RWLock.h"
#include "../Condition.h"
#include "../ReadLocker.h"
#include "../WriteLocker.h"

#include <atomic>

namespace Concurrent
{
	/**
	 * @internal
	 */
	template<typename T>
	struct ProducerInternal
	{
		struct WaitRecord
		{
			bool success;
			Condition* wakeUp;
			std::optional<T> out;
		};

		std::atomic<bool> endCalled;
		RWLock rwLock;

		Queue<T> messages;
		Queue<WaitRecord*> waiting;

		void pushMessage(const T &item)
		{
			WriteLocker lock(&rwLock);
			messages.push(item);

			WaitRecord* record;
			if (false == messages.isEmpty() && waiting.tryPop(record))
			{
				messages.tryPop(record->out);
				record->success = true;
				record->wakeUp->trigger();
			}
		}

		void pushMessage(T&& item)
		{
			WriteLocker lock(&rwLock);
			messages.push(std::move(item));

			WaitRecord* record;
			if (false == messages.isEmpty() && waiting.tryPop(record))
			{
				messages.tryPop(record->out);
				record->success = true;
				record->wakeUp->trigger();
			}
		}

		bool getMessage(std::optional<T>& out, bool trying = false)
		{
			Condition ready;
			WaitRecord record;
			{
				ReadLocker lock(&rwLock);

				if (messages.tryPop(out))
					return true;
				else if (trying || endCalled)
					return false;

				record.wakeUp = &ready;

				waiting.push(&record);
			}

			ready.wait();

			if (record.success)
			{
				out = std::move(record.out);
				return true;
			}

			return false;
		}

		bool getMessage(T &out, bool trying = false)
		{
			std::optional opt;

			if (getMessage(opt, trying))
			{
				out = std::move(*opt);
				return true;
			}

			return false;
		}

		void end()
		{
			WriteLocker lock(&rwLock);
			endCalled.store(true);

			WaitRecord* record;
			while (waiting.tryPop(record))
			{
				record->success = messages.tryPop(*record->out);
				record->wakeUp->trigger();
			}
		}
	};
}

#endif //_CONCURRENT_PRODUCE_CONSUME_INTERNAL_H_