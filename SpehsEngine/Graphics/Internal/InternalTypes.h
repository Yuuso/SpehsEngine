#pragma once

// NOTE: No SpehsEngine includes allowed


namespace se
{
	namespace graphics
	{
		struct RenderContext
		{
			const class WindowInternal* currentWindow	= nullptr;
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
			MinSizeChanged			= (1 << 2),
			MaxSizeChanged			= (1 << 3),
			ResizableChanged		= (1 << 4),
			Minimized				= (1 << 5),
			Maximized				= (1 << 6),
			Restored				= (1 << 7),
			InputFocused			= (1 << 8),
			Raised					= (1 << 9),
			NameChanged				= (1 << 10),
			FullscreenChanged		= (1 << 11),
			BorderlessChanged		= (1 << 12),
			ConfinedInputChanged	= (1 << 13),
			ShownChanged			= (1 << 14),
			OpacityChanged			= (1 << 15),
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

		enum class PrimitiveAttributeFlag: PrimitiveAttributeFlagsType
		{
			RenderState				= (1 << 0),
			Material				= (1 << 1),
			Instances				= (1 << 2),
			Color					= (1 << 3),
			RenderFlags				= (1 << 4),
			PrimitiveType			= (1 << 5),
			RenderMode				= (1 << 6),
		};
	}
}
