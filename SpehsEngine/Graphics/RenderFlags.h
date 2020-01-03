#pragma once

#include <stdint.h>


namespace se
{
	namespace graphics
	{
		typedef uint32_t RenderFlagsType;
		enum RenderFlag : RenderFlagsType
		{
			useMeshColor = (1 << 0),
			useMeshAlpha = (1 << 1),
			backFaceCulling = (1 << 2),
			renderState = (1 << 3),
			blending = (1 << 4),
			depthTest = (1 << 5),
			smoothLine = (1 << 6),
		};
	}
}
