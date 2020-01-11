#pragma once

#include "bgfx/bgfx.h" // TODO
#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/Graphics/Shader.h"


namespace se
{
	namespace graphics
	{
		struct RenderContext
		{
			const class WindowInstance* currentWindow	= nullptr;
			bgfx::ViewId currentViewId					= 0;
			RendererFlagsType rendererFlags				= 0;
			bool enableMSAA								= true;
		};

		enum UpdateFlag : UpdateFlagsType
		{
			NothingChanged		= 0,
			TransformChanged	= (1 << 0),
			RenderInfoChanged	= (1 << 1),
			VerticesChanged		= (1 << 2),
			IndicesChanged		= (1 << 3),
		};

		struct RenderInfo
		{
			RenderFlagsType renderFlags;
			PrimitiveType primitiveType;
			const Shader* shader;
		};

		struct BatchPosition
		{
			size_t verticesStart;
			size_t verticesEnd;
			size_t indicesStart;
			size_t indicesEnd;
		};
	}
}
