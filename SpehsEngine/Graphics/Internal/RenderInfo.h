#pragma once

#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Graphics/Material.h"
#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/Graphics/VertexAttribute.h"
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
