#pragma once

namespace se
{
	namespace graphics
	{
		class Camera;
		class Font;
		class FontManager;
		class ModelData;
		class ModelDataManager;
		class Primitive;
		class Renderer;
		class Scene;
		class Shader;
		class ShaderManager;
		class Shape;
		class ShapeGenerator;
		class Text;
		class Texture;
		class TextureManager;
		class View;
		class Window;
		struct FontMetrics;
		struct GlyphMetrics;
		struct ResourceData;
		typedef AsyncTaskManager<std::shared_ptr<ResourceData>> ResourceLoader;
	}
}

#include "SpehsEngine/Graphics/ResourceHandle.h"
#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/Graphics/Material.h"

static constexpr uint16_t SE_MAX_BONES = 32;	// BGFX_CONFIG_MAX_BONES
static constexpr size_t SE_MAX_LIGHTS = 16;
