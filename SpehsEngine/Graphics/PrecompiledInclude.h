#pragma once

namespace se
{
	namespace graphics
	{
		class Window;
		class Renderer;
		class Scene;
		class Camera;
		class View;
		class ShaderManager;
		class TextureManager;
		class FontManager;
		class ModelDataManager;
		class ShapeGenerator;
		struct ResourceData;
	}
}

#include "SpehsEngine/Graphics/ResourceHandle.h"
#include "SpehsEngine/Graphics/Types.h"

static constexpr uint16_t SE_MAX_BONES = 32;	// BGFX_CONFIG_MAX_BONES
static constexpr size_t SE_MAX_LIGHTS = 16;
