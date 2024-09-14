#pragma once

#include "SpehsEngine/Core/Thread.h"
#include <condition_variable>
#include <thread>
#include <queue>


namespace se
{
	template <typename T>
	class AsyncTaskManager
	{
	public:

		AsyncTaskManager(const size_t _numThreads = 1);
		~AsyncTaskManager();

		AsyncTaskManager(const AsyncTaskManager& _other) = delete;
		AsyncTaskManager& operator=(const AsyncTaskManager& _other) = delete;

		AsyncTaskManager(AsyncTaskManager&& _other) = delete;
		AsyncTaskManager& operator=(AsyncTaskManager&& _other) = delete;


		std::future<T> push(std::function<T()> _fn);
		size_t numTasksLeft() const;

	private:

		std::vector<std::thread> threadPool;

		mutable std::mutex mutex;
		std::condition_variable cvar;
		bool shutdown = false;
		size_t numWorking = 0;
		std::queue<std::packaged_task<T()>> taskQueue;
	};



	template <typename T>
	AsyncTaskManager<T>::AsyncTaskManager(const size_t _numThreads)
	{
		se_assert(_numThreads > 0);
		threadPool.resize(_numThreads);

		auto worker = [this]()
		{
			std::unique_lock<std::mutex> lock(mutex);
			while (true)
			{
				cvar.wait(lock, [&]{ return shutdown || !taskQueue.empty(); });

				if (shutdown)
					break;

				std::packaged_task<T()> task = std::move(taskQueue.front());
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

	template <typename T>
	AsyncTaskManager<T>::~AsyncTaskManager()
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

	template <typename T>
	std::future<T> AsyncTaskManager<T>::push(std::function<T()> _fn)
	{
		std::lock_guard<std::mutex> lock(mutex);
		taskQueue.emplace(_fn);
		cvar.notify_one();
		return taskQueue.back().get_future();
	}

	template <typename T>
	size_t AsyncTaskManager<T>::numTasksLeft() const
	{
		std::lock_guard<std::mutex> lock(mutex);
		return taskQueue.size() + numWorking;
	}
}
