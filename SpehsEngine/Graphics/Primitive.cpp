#include "stdafx.h"
#include "SpehsEngine/Graphics/Primitive.h"

#include "SpehsEngine/Core/BitwiseOperations.h"


namespace se
{
	namespace graphics
	{
		Primitive::Primitive()
		{
			enableBit(renderFlags, RenderFlag::renderState);
			enableBit(renderFlags, RenderFlag::backFaceCulling);
			enableBit(renderFlags, RenderFlag::depthTest);
		}
		Primitive::~Primitive()
		{

		}
	}
}
