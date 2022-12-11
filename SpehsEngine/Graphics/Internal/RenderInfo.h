#pragma once

#include "SpehsEngine/Graphics/VertexAttribute.h"


namespace se
{
	namespace graphics
	{
		class Material;

		struct RenderInfo
		{
			RenderFlagsType renderFlags;
			PrimitiveType primitiveType;
			std::shared_ptr<Material> material;
			VertexAttributeFlagsType attributes;
			Color primitiveColor;
			uint32_t depth = 0;
		};

		inline bool operator==(const RenderInfo& _left, const RenderInfo& _right)
		{
			return _left.renderFlags	== _right.renderFlags
				&& _left.primitiveType	== _right.primitiveType
				&& _left.material		== _right.material
				&& _left.attributes		== _right.attributes
				&& _left.primitiveColor == _right.primitiveColor
				&& _left.depth			== _right.depth
				;
		}
	}
}
