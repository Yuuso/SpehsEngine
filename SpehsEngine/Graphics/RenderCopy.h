#pragma once


namespace se
{
	namespace graphics
	{
		class Material;

		struct RenderCopy
		{
			std::optional<RenderFlagsType> renderFlags;
			std::optional<PrimitiveType> primitiveType;
			std::shared_ptr<Material> material;
			std::optional<Color> primitiveColor;
			std::optional<uint32_t> renderDepth;
		};
	}
}
