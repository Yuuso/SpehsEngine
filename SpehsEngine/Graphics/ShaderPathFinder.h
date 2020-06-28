#pragma once

#include "SpehsEngine/Graphics/Types.h"
#include <string>


namespace se
{
	namespace graphics
	{
		class ShaderPathFinder
		{
		public:
			virtual std::string getShaderPath(const RendererBackend _rendererBackend) const
			{
				switch (_rendererBackend)
				{
					case se::graphics::RendererBackend::Direct3D9:	return "data/shaders/dx9/";
					case se::graphics::RendererBackend::Direct3D11: return "data/shaders/dx11/";
					case se::graphics::RendererBackend::Metal:		return "data/shaders/metal/";
					case se::graphics::RendererBackend::OpenGLES:	[[fallthrough]];
					case se::graphics::RendererBackend::OpenGL:		return "data/shaders/glsl/";
					case se::graphics::RendererBackend::Vulkan:		return "data/shaders/spirv/";

					default:
						se_assert_m(false, "Unknown RendererBackend: " + std::to_string((int)_rendererBackend));
						return "";
				}
			}
		};
	}
}
