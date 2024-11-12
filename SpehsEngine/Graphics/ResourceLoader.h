#pragma once


namespace se
{
	namespace graphics
	{
		std::shared_ptr<ResourceLoader> makeResourceLoader(const size_t _numThreads = 1);
	}
}
