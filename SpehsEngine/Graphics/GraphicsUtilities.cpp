#include "stdafx.h"
#include "SpehsEngine/Graphics/GraphicsUtilities.h"


namespace se
{
	namespace graphics
	{
		std::string toString(RendererBackend _backend)
		{
			switch (_backend)
			{
				case se::graphics::RendererBackend::Auto:
				case se::graphics::RendererBackend::Noop:
					break;

				case se::graphics::RendererBackend::Agc:			return "Arc";
				case se::graphics::RendererBackend::Direct3D9:		return "Direct3D 9";
				case se::graphics::RendererBackend::Direct3D11:		return "Direct3D 11";
				case se::graphics::RendererBackend::Direct3D12:		return "Direct3D 12";
				case se::graphics::RendererBackend::Gnm:			return "Gnm";
				case se::graphics::RendererBackend::Metal:			return "Metal";
				case se::graphics::RendererBackend::Nvn:			return "Nvn";
				case se::graphics::RendererBackend::OpenGLES:		return "OpenGL ES";
				case se::graphics::RendererBackend::OpenGL:			return "OpenGL";
				case se::graphics::RendererBackend::Vulkan:			return "Vulkan";
				case se::graphics::RendererBackend::WebGPU:			return "WebGPU";
			}
			se_assert(false);
			return "Unknown";
		}
	}
}
