#pragma once

#include "bgfx/bgfx.h" // TODO
#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/Graphics/Material.h"


namespace se
{
	namespace graphics
	{
		struct RenderContext
		{
			const class WindowInstance* currentWindow	= nullptr;
			class DefaultUniforms* defaultUniforms		= nullptr;
			bgfx::ViewId currentViewId					= 0;
			RendererFlagsType rendererFlags				= 0;
			bool enableMSAA								= true;
		};

		enum PrimitiveUpdateFlag : PrimitiveUpdateFlagsType
		{
			TransformChanged	= (1 << 0),
			RenderInfoChanged	= (1 << 1),
			VerticesChanged		= (1 << 2),
			IndicesChanged		= (1 << 3),
		};

		enum WindowUpdateFlag : WindowUpdateFlagsType
		{
			PositionChanged			= (1 << 0),
			SizeChanged				= (1 << 1),
			ResizableChanged		= (1 << 2),
			Minimized				= (1 << 3),
			Maximized				= (1 << 4),
			Restored				= (1 << 5),
			InputFocused			= (1 << 6),
			Raised					= (1 << 7),
			NameChanged				= (1 << 8),
			BorderlessChanged		= (1 << 9),
			ConfinedInputChanged	= (1 << 10),
			ShownChanged			= (1 << 11),
			OpacityChanged			= (1 << 12),
		};

		struct RenderInfo
		{
			RenderFlagsType renderFlags;
			PrimitiveType primitiveType;
			std::shared_ptr<Material> material;
			VertexAttributeFlagsType attributes;
		};
		inline bool operator==(const RenderInfo& _left, const RenderInfo& _right)
		{
			return _left.renderFlags == _right.renderFlags
				&& _left.primitiveType == _right.primitiveType
				&& _left.material == _right.material
				&& _left.attributes == _right.attributes;
		}

		struct BatchPosition
		{
			size_t verticesStart;
			size_t verticesEnd;
			size_t indicesStart;
			size_t indicesEnd;
		};
	}
}
