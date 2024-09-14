#include "stdafx.h"
#include "SpehsEngine/Audio/ResourceLoader.h"

#include "SpehsEngine/Core/AsyncTaskManager.h"


namespace se
{
	namespace audio
	{
		std::shared_ptr<ResourceLoader> makeResourceLoader(const size_t _numThreads)
		{
			return std::make_shared<ResourceLoader>(_numThreads);
		}
	}
}
