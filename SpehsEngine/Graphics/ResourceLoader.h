#pragma once

#include <memory>
#include "SpehsEngine/Core/AsyncTaskManager.h"
#include "SpehsEngine/Graphics/ResourceData.h"


namespace se
{
	namespace graphics
	{
		typedef AsyncTaskManager<std::shared_ptr<ResourceData>> ResourceLoader;

		inline std::shared_ptr<ResourceLoader> makeResourceLoader(const size_t _numThreads = 1)
		{
			std::shared_ptr<ResourceLoader> result = std::make_shared<ResourceLoader>(_numThreads);
			return result;
		}
	}
}
