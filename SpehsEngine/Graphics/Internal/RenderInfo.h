#pragma once

#include "SpehsEngine/Graphics/Material.h"
#include "SpehsEngine/Graphics/Types.h"
#include <memory>


namespace se
{
	namespace graphics
	{
		struct RenderInfo
		{
			RenderFlagsType renderFlags;
			PrimitiveType primitiveType;
			std::shared_ptr<Material> material;
			VertexAttributeFlagsType attributes;
		};

		inline bool operator==(const RenderInfo& _left, const RenderInfo& _right)
		{
			return _left.renderFlags	== _right.renderFlags
				&& _left.primitiveType	== _right.primitiveType
				&& _left.material		== _right.material
				&& _left.attributes		== _right.attributes;
		}
	}
}
