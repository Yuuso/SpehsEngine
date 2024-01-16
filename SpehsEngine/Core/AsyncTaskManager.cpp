#include "stdafx.h"
#include "SpehsEngine/Core/AsyncTaskManager.h"


namespace se
{
	AsyncTaskManager::AsyncTaskManager(size_t _threadCount)
	{
		se_assert(_threadCount > 0);
		threadPool.resize(std::max(_threadCount, size_t{ 1 }));

		auto worker =
			[this]()
			{
				std::unique_lock<std::mutex> lock(mutex);
				while (true)
				{
					cvar.wait(lock, [&]
							  {
								  return shutdown || !taskQueue.empty();
							  });

					if (shutdown)
						break;

					std::function<void()> task = std::move(taskQueue.front());
					taskQueue.pop();

					numWorking++;
					lock.unlock();
					task();
					lock.lock();
					numWorking--;
				}
			};

		unsigned threadNum = 0;
		for (auto&& thread : threadPool)
		{
			thread = std::thread(worker);
			setThreadName(thread, "AsyncTaskManagerWorker" + std::to_string(++threadNum));
		}
	}
	AsyncTaskManager::~AsyncTaskManager()
	{
		{
			std::lock_guard<std::mutex> lock(mutex);
			shutdown = true;
		}
		cvar.notify_all();

		for (auto&& thread : threadPool)
		{
			thread.join();
		}
	}
	void AsyncTaskManager::add(const std::function<void()>& _fn)
	{
		std::lock_guard<std::mutex> lock(mutex);
		taskQueue.emplace(_fn);
		cvar.notify_one();
	}
	size_t AsyncTaskManager::getTasksLeft() const
	{
		std::lock_guard<std::mutex> lock(mutex);
		return taskQueue.size() + numWorking;
	}
	size_t AsyncTaskManager::getThreadCount() const
	{
		return threadPool.size();
	}
	size_t AsyncTaskManager::getDefaultThreadCount()
	{
		const size_t cpuCount = std::thread::hardware_concurrency();
		return std::max(cpuCount, size_t{ 2 }) - size_t{ 1 };
	}
}