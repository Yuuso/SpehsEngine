#include "stdafx.h"
#include "SpehsEngine/Graphics/GraphicsUtilities.h"


namespace se::gfx
{
	std::string toString(RendererBackend _backend)
	{
		switch (_backend)
		{
			case RendererBackend::Auto:
			case RendererBackend::Noop:
				break;

			case RendererBackend::Agc:			return "Arc";
			case RendererBackend::Direct3D9:	return "Direct3D 9";
			case RendererBackend::Direct3D11:	return "Direct3D 11";
			case RendererBackend::Direct3D12:	return "Direct3D 12";
			case RendererBackend::Gnm:			return "Gnm";
			case RendererBackend::Metal:		return "Metal";
			case RendererBackend::Nvn:			return "Nvn";
			case RendererBackend::OpenGLES:		return "OpenGL ES";
			case RendererBackend::OpenGL:		return "OpenGL";
			case RendererBackend::Vulkan:		return "Vulkan";
			case RendererBackend::WebGPU:		return "WebGPU";
		}
		se_assert(false);
		return "Unknown";
	}
}
