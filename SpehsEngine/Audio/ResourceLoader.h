#pragma once

#include "SpehsEngine/Audio/Internal/InternalTypes.h"


namespace se
{
	namespace audio
	{
		struct AudioResourceData
		{
			std::shared_ptr<AudioSourceBase> source;
			time::Time length;
		};

		std::shared_ptr<ResourceLoader> makeResourceLoader(const size_t _numThreads = 1);
	}
}
