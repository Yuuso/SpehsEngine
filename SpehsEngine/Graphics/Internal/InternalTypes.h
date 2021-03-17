#pragma once

// NOTE: No SpehsEngine includes allowed (other than Types.h)

#include "bgfx/bgfx.h" // TODO
#include "glm/mat4x4.hpp"
#include "SpehsEngine/Graphics/Types.h"
#include <vector>


namespace se
{
	namespace graphics
	{
		struct RenderContext
		{
			const class WindowInstance* currentWindow	= nullptr;
			class DefaultUniforms* defaultUniforms		= nullptr;
			class LightBatch* lightBatch				= nullptr;
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
			EverythingChanged	= std::numeric_limits<PrimitiveUpdateFlagsType>::max()
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

		struct BatchPosition
		{
			size_t verticesStart;
			size_t verticesEnd;
			size_t indicesStart;
			size_t indicesEnd;
		};

		enum class TextureStatus
		{
			Init,
			Loading,
			Valid,
			Error,
		};

		typedef std::vector<glm::mat4> UniformMatrices;
	}
}
