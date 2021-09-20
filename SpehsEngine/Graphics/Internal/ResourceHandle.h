#pragma once

#include <stdint.h>


namespace se
{
	namespace graphics
	{
		typedef uint16_t ResourceHandle;
		constexpr ResourceHandle INVALID_RESOURCE_HANDLE = UINT16_MAX;
	}
}
