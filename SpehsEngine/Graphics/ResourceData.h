#pragma once

#include <stdint.h>
#include <memory>
#include "SpehsEngine/Core/AsyncTaskManager.h"
#include "bgfx/bgfx.h"


namespace se
{
	namespace graphics
	{
		typedef uint16_t ResourceHandle;
		constexpr ResourceHandle INVALID_RESOURCE_HANDLE = bgfx::kInvalidHandle;

		struct ResourceData
		{
			virtual ~ResourceData() = default;
			ResourceHandle handle = INVALID_RESOURCE_HANDLE;
		};

		typedef std::shared_ptr<AsyncTaskManager<std::shared_ptr<ResourceData>>> ResourceLoader;
	}
}
