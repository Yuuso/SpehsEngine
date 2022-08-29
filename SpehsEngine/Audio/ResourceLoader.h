#pragma once

#include <memory>
#include "SpehsEngine/Audio/Internal/InternalTypes.h"
#include "SpehsEngine/Core/AsyncTaskManager.h"
#include "SpehsEngine/Core/SE_Time.h"


namespace se
{
	namespace audio
	{
		struct AudioResourceData
		{
			std::shared_ptr<AudioSourceBase> source;
			se::time::Time length;
		};

		typedef AsyncTaskManager<AudioResourceData> ResourceLoader;

		inline std::shared_ptr<ResourceLoader> makeResourceLoader(const size_t _numThreads = 1)
		{
			return std::make_shared<ResourceLoader>(_numThreads);
		}
	}
}
