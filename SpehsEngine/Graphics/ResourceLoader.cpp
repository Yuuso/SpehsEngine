#include "stdafx.h"
#include "SpehsEngine/Graphics/ResourceLoader.h"

#include "SpehsEngine/Core/AsyncTaskManager.h"
#include "SpehsEngine/Graphics/ResourceData.h"


namespace se
{
	namespace graphics
	{
		std::shared_ptr<ResourceLoader> makeResourceLoader(const size_t _numThreads)
		{
			std::shared_ptr<ResourceLoader> result = std::make_shared<ResourceLoader>(_numThreads);
			return result;
		}
	}
}
