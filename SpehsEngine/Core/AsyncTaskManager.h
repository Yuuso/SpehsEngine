#pragma once

#include "SpehsEngine/Core/Thread.h"


namespace se
{
	class AsyncTaskManager
	{
	public:

		SE_NO_COPY_OR_MOVE(AsyncTaskManager)
		AsyncTaskManager(size_t _threadCount = getDefaultThreadCount());
		~AsyncTaskManager();

		// Add task to queue
		void add(const std::function<void()>& _fn);

		// Add task with return type to queue
		template<typename T>
		std::future<T> add(const std::function<T()>& _fn)
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::shared_ptr<std::packaged_task<T()>> task =
				std::make_shared<std::packaged_task<T()>>(_fn);
			taskQueue.emplace([task]{ (*task)(); });
			cvar.notify_one();
			return task->get_future();
		}

		// Returns the number of unfinished tasks
		size_t getTasksLeft() const;

		// Returns number of threads in thread pool
		size_t getThreadCount() const;

		// Returns recommended default thread count
		static size_t getDefaultThreadCount();

	private:

		mutable std::mutex mutex;
		std::condition_variable cvar;
		bool shutdown = false;
		size_t numWorking = 0;
		std::queue<std::function<void()>> taskQueue;
		std::vector<std::thread> threadPool;
	};
}
