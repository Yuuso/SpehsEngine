#pragma once

#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/Core/Color.h"


namespace se
{
	namespace graphics
	{
		struct RenderCopy
		{
			RenderFlagsType renderFlags;
			PrimitiveType primitiveType;
			Color primitiveColor;
		};
	}
}
