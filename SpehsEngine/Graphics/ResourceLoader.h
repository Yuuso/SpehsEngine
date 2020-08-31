#pragma once

#include <memory>
#include "SpehsEngine/Core/AsyncTaskManager.h"
#include "SpehsEngine/Graphics/ResourceData.h"


namespace se
{
	namespace graphics
	{
		typedef AsyncTaskManager<std::shared_ptr<ResourceData>> ResourceLoaderType;
		typedef std::shared_ptr<ResourceLoaderType> ResourceLoader;

		inline ResourceLoader makeResourceLoader(const size_t _numThreads = 1)
		{
			std::shared_ptr<ResourceLoaderType> result = std::make_shared<ResourceLoaderType>(_numThreads);
			return result;
		}
	}
}
