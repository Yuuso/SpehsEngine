#pragma once

#include <memory>
#include <SpehsEngine/Graphics/ResourceHandle.h>


namespace se
{
	namespace graphics
	{
		struct ResourceData
		{
			virtual ~ResourceData() = default;
			ResourceHandle handle = INVALID_RESOURCE_HANDLE;
		};
	}
}
