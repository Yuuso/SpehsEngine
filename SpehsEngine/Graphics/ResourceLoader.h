#pragma once


namespace se
{
	namespace graphics
	{
		typedef AsyncTaskManager<std::shared_ptr<ResourceData>> ResourceLoader;

		std::shared_ptr<ResourceLoader> makeResourceLoader(const size_t _numThreads = 1);
	}
}
