#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/InternalUtilities.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Graphics/Primitive.h"


namespace se
{
	namespace graphics
	{
		void applyRenderState(const RenderInfo& _renderInfo, const RenderContext& _renderContext)
		{
			uint64_t state = BGFX_STATE_NONE;

			state |= BGFX_STATE_WRITE_MASK; // TODO

			if (_renderContext.enableMSAA)
				state |= BGFX_STATE_MSAA;

			if (checkBit(_renderInfo.renderFlags, RenderFlag::DepthTest)) // TODO
				state |= BGFX_STATE_DEPTH_TEST_LESS;

			if (checkBit(_renderInfo.renderFlags, RenderFlag::Blending)) // TODO
				state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);

			if (!checkBit(_renderContext.rendererFlags, RendererFlag::CWFrontFace))
				state |= BGFX_STATE_FRONT_CCW;

			const bool cullBackface = checkBit(_renderInfo.renderFlags, RenderFlag::CullBackFace);
			const bool cullFrontface = checkBit(_renderInfo.renderFlags, RenderFlag::CullFrontFace);
			se_assert_m(!(cullBackface && cullFrontface), "Both frontface and backface culling are enabled!?");

			if (cullBackface)
				state |= checkBit(_renderContext.rendererFlags, RendererFlag::CWFrontFace) ? BGFX_STATE_CULL_CCW : BGFX_STATE_CULL_CW;
			else if (cullFrontface)
				state |= checkBit(_renderContext.rendererFlags, RendererFlag::CWFrontFace) ? BGFX_STATE_CULL_CW : BGFX_STATE_CULL_CCW;

			switch (_renderInfo.primitiveType)
			{
				default:
					se_assert_m(false, "Unknown primitive type!");
					[[fallthrough]];
				case PrimitiveType::Triangles:
					// Default
					break;
				case PrimitiveType::Lines:
					state |= BGFX_STATE_PT_LINES;
					break;
				case PrimitiveType::Points:
					state |= BGFX_STATE_PT_POINTS;
					break;
			}

			// TODO: BGFX_STATE_POINT_SIZE
			// TODO: BGFX_STATE_LINEAA
			// TODO: BGFX_STATE_CONSERVATIVE_RASTER

			bgfx::setState(state);
		}

		const bgfx::VertexLayout findVertexLayout(const VertexAttributeFlagsType _attributes)
		{
			bgfx::VertexLayout layout;
			layout.begin();
			using namespace VertexAttribute;
			if(checkBit(_attributes, Position))		layout.add(bgfx::Attrib::Position,		3, bgfx::AttribType::Float);
			if(checkBit(_attributes, Normal))		layout.add(bgfx::Attrib::Normal,		3, bgfx::AttribType::Float);
			if(checkBit(_attributes, Tangent))		layout.add(bgfx::Attrib::Tangent,		3, bgfx::AttribType::Float);
			if(checkBit(_attributes, Bitangent))	layout.add(bgfx::Attrib::Bitangent,		3, bgfx::AttribType::Float);
			if(checkBit(_attributes, Color0))		layout.add(bgfx::Attrib::Color0,		4, bgfx::AttribType::Float);
			if(checkBit(_attributes, Color1))		layout.add(bgfx::Attrib::Color1,		4, bgfx::AttribType::Float);
			if(checkBit(_attributes, Color2))		layout.add(bgfx::Attrib::Color2,		4, bgfx::AttribType::Float);
			if(checkBit(_attributes, Color3))		layout.add(bgfx::Attrib::Color3,		4, bgfx::AttribType::Float);
			if(checkBit(_attributes, Weight))		layout.add(bgfx::Attrib::Weight,		4, bgfx::AttribType::Float);
			if(checkBit(_attributes, Indices))		layout.add(bgfx::Attrib::Indices,		4, bgfx::AttribType::Int16);
			if(checkBit(_attributes, TexCoord0))	layout.add(bgfx::Attrib::TexCoord0,		2, bgfx::AttribType::Float);
			if(checkBit(_attributes, TexCoord1))	layout.add(bgfx::Attrib::TexCoord1,		2, bgfx::AttribType::Float);
			if(checkBit(_attributes, TexCoord2))	layout.add(bgfx::Attrib::TexCoord2,		2, bgfx::AttribType::Float);
			if(checkBit(_attributes, Data0))		layout.add(bgfx::Attrib::TexCoord3,		4, bgfx::AttribType::Float);
			if(checkBit(_attributes, Data1))		layout.add(bgfx::Attrib::TexCoord4,		4, bgfx::AttribType::Float);
			if(checkBit(_attributes, Data2))		layout.add(bgfx::Attrib::TexCoord5,		4, bgfx::AttribType::Float);
			if(checkBit(_attributes, Data3))		layout.add(bgfx::Attrib::TexCoord6,		4, bgfx::AttribType::Float);
			if(checkBit(_attributes, Data4))		layout.add(bgfx::Attrib::TexCoord7,		4, bgfx::AttribType::Float);
			layout.end();
			return layout;
		}

		uint64_t TextureModesToFlags(const TextureModes _textureModes)
		{
			uint64_t flags = BGFX_TEXTURE_NONE;
			switch (_textureModes.sampleMag)
			{
				case TextureSamplingMode::Linear:
					// default
					break;
				case TextureSamplingMode::Point:
					flags |= BGFX_SAMPLER_MAG_POINT;
					break;
				case TextureSamplingMode::Anisotrophic:
					flags |= BGFX_SAMPLER_MAG_ANISOTROPIC;
					break;
			}
			switch (_textureModes.sampleMin)
			{
				case TextureSamplingMode::Linear:
					// default
					break;
				case TextureSamplingMode::Point:
					flags |= BGFX_SAMPLER_MIN_POINT;
					break;
				case TextureSamplingMode::Anisotrophic:
					flags |= BGFX_SAMPLER_MIN_ANISOTROPIC;
					break;
			}
			switch (_textureModes.sampleMip)
			{
				case TextureSamplingMode::Linear:
					// default
					break;
				case TextureSamplingMode::Point:
					flags |= BGFX_SAMPLER_MIP_POINT;
					break;
				case TextureSamplingMode::Anisotrophic:
					se_assert_m(false, "Invalid TextureSamplingMode: anisotrophic not allowed for mip sampling!");
					break;
			}

			switch (_textureModes.wrapU)
			{
				case se::graphics::TextureWrappingMode::Repeat:
					// default
					break;
				case se::graphics::TextureWrappingMode::Mirror:
					flags |= BGFX_SAMPLER_U_MIRROR;
					break;
				case se::graphics::TextureWrappingMode::Clamp:
					flags |= BGFX_SAMPLER_U_CLAMP;
					break;
				case se::graphics::TextureWrappingMode::Border:
					flags |= BGFX_SAMPLER_U_BORDER;
					break;
			}
			switch (_textureModes.wrapV)
			{
				case se::graphics::TextureWrappingMode::Repeat:
					// default
					break;
				case se::graphics::TextureWrappingMode::Mirror:
					flags |= BGFX_SAMPLER_V_MIRROR;
					break;
				case se::graphics::TextureWrappingMode::Clamp:
					flags |= BGFX_SAMPLER_V_CLAMP;
					break;
				case se::graphics::TextureWrappingMode::Border:
					flags |= BGFX_SAMPLER_V_BORDER;
					break;
			}
			switch (_textureModes.wrapW)
			{
				case se::graphics::TextureWrappingMode::Repeat:
					// default
					break;
				case se::graphics::TextureWrappingMode::Mirror:
					flags |= BGFX_SAMPLER_W_MIRROR;
					break;
				case se::graphics::TextureWrappingMode::Clamp:
					flags |= BGFX_SAMPLER_W_CLAMP;
					break;
				case se::graphics::TextureWrappingMode::Border:
					flags |= BGFX_SAMPLER_W_BORDER;
					break;
			}

			return flags;
		}

		uint32_t getResetParameters(const RendererFlagsType _rendererFlags)
		{
			uint32_t result = BGFX_RESET_NONE;
			auto updateFlag =
				[&result, _rendererFlags](const RendererFlag _flag, const uint32_t _param) -> void
			{
				if (checkBit(_rendererFlags, _flag))
					result |= _param;
			};

			updateFlag(RendererFlag::VSync, BGFX_RESET_VSYNC);
			updateFlag(RendererFlag::MSAA2, BGFX_RESET_MSAA_X2);
			updateFlag(RendererFlag::MSAA4, BGFX_RESET_MSAA_X4);
			updateFlag(RendererFlag::MSAA8, BGFX_RESET_MSAA_X8);
			updateFlag(RendererFlag::MSAA16, BGFX_RESET_MSAA_X16);
			//updateFlag(RendererFlag::?, BGFX_RESET_MAXANISOTROPY);
			//updateFlag(RendererFlag::?, BGFX_RESET_FULLSCREEN);
			//updateFlag(RendererFlag::?, BGFX_RESET_SRGB_BACKBUFFER);
			//updateFlag(RendererFlag::?, BGFX_RESET_HDR10);
			//updateFlag(RendererFlag::?, BGFX_RESET_HIDPI);
			//updateFlag(RendererFlag::?, BGFX_RESET_DEPTH_CLAMP);
			//updateFlag(RendererFlag::?, BGFX_RESET_SUSPEND);
			//updateFlag(RendererFlag::?, BGFX_RESET_FLUSH_AFTER_RENDER);
			//updateFlag(RendererFlag::?, BGFX_RESET_FLIP_AFTER_RENDER);

			return result;
		}

		bgfx::RendererType::Enum getRendererType(const RendererBackend _rendererBackend)
		{
			switch (_rendererBackend)
			{
				case RendererBackend::Auto:			return bgfx::RendererType::Count;
				case RendererBackend::Direct3D9:	return bgfx::RendererType::Direct3D9;
				case RendererBackend::Direct3D11:	return bgfx::RendererType::Direct3D11;
				case RendererBackend::Direct3D12:	return bgfx::RendererType::Direct3D12;
				case RendererBackend::Gnm:			return bgfx::RendererType::Gnm;
				case RendererBackend::Metal:		return bgfx::RendererType::Metal;
				case RendererBackend::Nvn:			return bgfx::RendererType::Nvn;
				case RendererBackend::OpenGLES:		return bgfx::RendererType::OpenGLES;
				case RendererBackend::OpenGL:		return bgfx::RendererType::OpenGL;
				case RendererBackend::Vulkan:		return bgfx::RendererType::Vulkan;
			}
			return bgfx::RendererType::Count;
		}

		RendererBackend getRendererBackend()
		{
			switch (bgfx::getRendererType())
			{
				case bgfx::RendererType::Count:
				case bgfx::RendererType::Noop:
				case bgfx::RendererType::WebGPU:		break;

				case bgfx::RendererType::Direct3D9:		return RendererBackend::Direct3D9;
				case bgfx::RendererType::Direct3D11:	return RendererBackend::Direct3D11;
				case bgfx::RendererType::Direct3D12:	return RendererBackend::Direct3D12;
				case bgfx::RendererType::Gnm:			return RendererBackend::Gnm;
				case bgfx::RendererType::Metal:			return RendererBackend::Metal;
				case bgfx::RendererType::Nvn:			return RendererBackend::Nvn;
				case bgfx::RendererType::OpenGLES:		return RendererBackend::OpenGLES;
				case bgfx::RendererType::OpenGL:		return RendererBackend::OpenGL;
				case bgfx::RendererType::Vulkan:		return RendererBackend::Vulkan;
			}
			se_assert_m(false, "Renderer type not defined?");
			return RendererBackend::Auto;
		}

		bgfx::UniformType::Enum getUniformType(const UniformType _type)
		{
			switch (_type)
			{
				case UniformType::Invalid:		break;
				case UniformType::Sampler:		return bgfx::UniformType::Sampler;
				case UniformType::Vec4:			return bgfx::UniformType::Vec4;
				case UniformType::Mat3:			return bgfx::UniformType::Mat3;
				case UniformType::Mat4:			return bgfx::UniformType::Mat4;
			}
			se_assert_m(false, "Invalid UniformType!");
			return bgfx::UniformType::Count;
		}
	}
}
