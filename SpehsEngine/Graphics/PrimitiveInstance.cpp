#include "stdafx.h"
#include "SpehsEngine/Graphics/PrimitiveInstance.h"


namespace se
{
	namespace graphics
	{
		PrimitiveInstance::PrimitiveInstance(const Primitive& _primitive)
			: primitive(_primitive)
		{
		}
		PrimitiveInstance::~PrimitiveInstance()
		{
		}

		bool PrimitiveInstance::operator==(const Primitive& _other)
		{
			return &_other == &primitive;
		}
	}
}
