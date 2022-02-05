#pragma once

#include <memory>
#include "SpehsEngine/Audio/Internal/InternalTypes.h"
#include "SpehsEngine/Core/AsyncTaskManager.h"


namespace se
{
	namespace audio
	{
		typedef AsyncTaskManager<std::shared_ptr<AudioSourceBase>> ResourceLoader;

		inline std::shared_ptr<ResourceLoader> makeResourceLoader(const size_t _numThreads = 1)
		{
			return std::make_shared<ResourceLoader>(_numThreads);
		}
	}
}
