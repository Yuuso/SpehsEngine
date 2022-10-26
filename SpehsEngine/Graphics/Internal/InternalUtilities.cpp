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

			state |= BGFX_STATE_WRITE_RGB;
			if (checkBit(_renderInfo.renderFlags, RenderFlag::WriteDepth))
				state |= BGFX_STATE_WRITE_Z;
			if (checkBit(_renderInfo.renderFlags, RenderFlag::WriteAlpha))
				state |= BGFX_STATE_WRITE_A;

			if (_renderContext.enableMSAA)
				state |= BGFX_STATE_MSAA;

			if (checkBit(_renderInfo.renderFlags, RenderFlag::DepthTestAlways))
				state |= BGFX_STATE_DEPTH_TEST_ALWAYS;
			if (checkBit(_renderInfo.renderFlags, RenderFlag::DepthTestNever))
				state |= BGFX_STATE_DEPTH_TEST_NEVER;
			if (checkBit(_renderInfo.renderFlags, RenderFlag::DepthTestEqual))
				state |= BGFX_STATE_DEPTH_TEST_EQUAL;
			if (checkBit(_renderInfo.renderFlags, RenderFlag::DepthTestNotEqual))
				state |= BGFX_STATE_DEPTH_TEST_NOTEQUAL;
			if (checkBit(_renderInfo.renderFlags, RenderFlag::DepthTestLess))
				state |= BGFX_STATE_DEPTH_TEST_LESS;
			if (checkBit(_renderInfo.renderFlags, RenderFlag::DepthTestLessOrEqual))
				state |= BGFX_STATE_DEPTH_TEST_LEQUAL;
			if (checkBit(_renderInfo.renderFlags, RenderFlag::DepthTestGreater))
				state |= BGFX_STATE_DEPTH_TEST_GREATER;
			if (checkBit(_renderInfo.renderFlags, RenderFlag::DepthTestGreaterOrEqual))
				state |= BGFX_STATE_DEPTH_TEST_GEQUAL;

			if (checkBit(_renderInfo.renderFlags, RenderFlag::BlendAlpha))
				state |= BGFX_STATE_BLEND_ALPHA | BGFX_STATE_WRITE_A;
			if (checkBit(_renderInfo.renderFlags, RenderFlag::BlendAdd))
				state |= BGFX_STATE_BLEND_ADD | BGFX_STATE_WRITE_A;
			if (checkBit(_renderInfo.renderFlags, RenderFlag::BlendDarken))
				state |= BGFX_STATE_BLEND_DARKEN | BGFX_STATE_WRITE_A;
			if (checkBit(_renderInfo.renderFlags, RenderFlag::BlendLighten))
				state |= BGFX_STATE_BLEND_LIGHTEN | BGFX_STATE_WRITE_A;
			if (checkBit(_renderInfo.renderFlags, RenderFlag::BlendMultiply))
				state |= BGFX_STATE_BLEND_MULTIPLY | BGFX_STATE_WRITE_A;
			if (checkBit(_renderInfo.renderFlags, RenderFlag::BlendNormal))
				state |= BGFX_STATE_BLEND_NORMAL | BGFX_STATE_WRITE_A;
			if (checkBit(_renderInfo.renderFlags, RenderFlag::BlendScreen))
				state |= BGFX_STATE_BLEND_SCREEN | BGFX_STATE_WRITE_A;
			if (checkBit(_renderInfo.renderFlags, RenderFlag::BlendBurn))
				state |= BGFX_STATE_BLEND_LINEAR_BURN | BGFX_STATE_WRITE_A;

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
				case RendererBackend::Noop:			return bgfx::RendererType::Noop;

				case RendererBackend::Agc:			return bgfx::RendererType::Agc;
				case RendererBackend::Direct3D9:	return bgfx::RendererType::Direct3D9;
				case RendererBackend::Direct3D11:	return bgfx::RendererType::Direct3D11;
				case RendererBackend::Direct3D12:	return bgfx::RendererType::Direct3D12;
				case RendererBackend::Gnm:			return bgfx::RendererType::Gnm;
				case RendererBackend::Metal:		return bgfx::RendererType::Metal;
				case RendererBackend::Nvn:			return bgfx::RendererType::Nvn;
				case RendererBackend::OpenGLES:		return bgfx::RendererType::OpenGLES;
				case RendererBackend::OpenGL:		return bgfx::RendererType::OpenGL;
				case RendererBackend::Vulkan:		return bgfx::RendererType::Vulkan;
				case RendererBackend::WebGPU:		return bgfx::RendererType::WebGPU;
			}
			return bgfx::RendererType::Count;
		}

		RendererBackend getRendererBackend()
		{
			switch (bgfx::getRendererType())
			{
				case bgfx::RendererType::Count:			return RendererBackend::Auto;
				case bgfx::RendererType::Noop:			return RendererBackend::Noop;

				case bgfx::RendererType::Agc:			return RendererBackend::Agc;
				case bgfx::RendererType::Direct3D9:		return RendererBackend::Direct3D9;
				case bgfx::RendererType::Direct3D11:	return RendererBackend::Direct3D11;
				case bgfx::RendererType::Direct3D12:	return RendererBackend::Direct3D12;
				case bgfx::RendererType::Gnm:			return RendererBackend::Gnm;
				case bgfx::RendererType::Metal:			return RendererBackend::Metal;
				case bgfx::RendererType::Nvn:			return RendererBackend::Nvn;
				case bgfx::RendererType::OpenGLES:		return RendererBackend::OpenGLES;
				case bgfx::RendererType::OpenGL:		return RendererBackend::OpenGL;
				case bgfx::RendererType::Vulkan:		return RendererBackend::Vulkan;
				case bgfx::RendererType::WebGPU:		return RendererBackend::WebGPU;
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

		bgfx::ViewMode::Enum getViewMode(DrawCallSortOrder _type)
		{
			switch (_type)
			{
				case DrawCallSortOrder::Default:			return bgfx::ViewMode::Default;
				case DrawCallSortOrder::Sequential:			return bgfx::ViewMode::Sequential;
				case DrawCallSortOrder::DepthAscending:		return bgfx::ViewMode::DepthAscending;
				case DrawCallSortOrder::DepthDescending:	return bgfx::ViewMode::DepthDescending;
			}
			se_assert_m(false, "Invalid DrawCallSortOrder!");
			return bgfx::ViewMode::Default;
		}

		static inline uint32_t setStencilTestState(const Stencil& _stencil)
		{
			switch (_stencil.test)
			{
				case StencilTest::Less:				return BGFX_STENCIL_TEST_LESS;
				case StencilTest::LessOrEqual:		return BGFX_STENCIL_TEST_LEQUAL;
				case StencilTest::Equal:			return BGFX_STENCIL_TEST_EQUAL;
				case StencilTest::GreaterOrEqual:	return BGFX_STENCIL_TEST_GEQUAL;
				case StencilTest::Greater:			return BGFX_STENCIL_TEST_GREATER;
				case StencilTest::NotEqual:			return BGFX_STENCIL_TEST_NOTEQUAL;
				case StencilTest::Never:			return BGFX_STENCIL_TEST_NEVER;
				case StencilTest::Always:			return BGFX_STENCIL_TEST_ALWAYS;
			}
			se_assert(false);
			return 0;
		}
		static inline uint32_t setStencilOperationsState(const Stencil& _stencil)
		{
			uint32_t result = 0;
			switch (_stencil.stencilFail)
			{
				case StencilOperation::Zero:				result |= BGFX_STENCIL_OP_FAIL_S_ZERO;		break;
				case StencilOperation::Keep:				result |= BGFX_STENCIL_OP_FAIL_S_KEEP;		break;
				case StencilOperation::Replace:				result |= BGFX_STENCIL_OP_FAIL_S_REPLACE;	break;
				case StencilOperation::Increment:			result |= BGFX_STENCIL_OP_FAIL_S_INCR;		break;
				case StencilOperation::IncrementSaturate:	result |= BGFX_STENCIL_OP_FAIL_S_INCRSAT;	break;
				case StencilOperation::Decrement:			result |= BGFX_STENCIL_OP_FAIL_S_DECR;		break;
				case StencilOperation::DecrementSaturate:	result |= BGFX_STENCIL_OP_FAIL_S_DECRSAT;	break;
				case StencilOperation::Invert:				result |= BGFX_STENCIL_OP_FAIL_S_INVERT;	break;
			}
			switch (_stencil.depthFail)
			{
				case StencilOperation::Zero:				result |= BGFX_STENCIL_OP_FAIL_Z_ZERO;		break;
				case StencilOperation::Keep:				result |= BGFX_STENCIL_OP_FAIL_Z_KEEP;		break;
				case StencilOperation::Replace:				result |= BGFX_STENCIL_OP_FAIL_Z_REPLACE;	break;
				case StencilOperation::Increment:			result |= BGFX_STENCIL_OP_FAIL_Z_INCR;		break;
				case StencilOperation::IncrementSaturate:	result |= BGFX_STENCIL_OP_FAIL_Z_INCRSAT;	break;
				case StencilOperation::Decrement:			result |= BGFX_STENCIL_OP_FAIL_Z_DECR;		break;
				case StencilOperation::DecrementSaturate:	result |= BGFX_STENCIL_OP_FAIL_Z_DECRSAT;	break;
				case StencilOperation::Invert:				result |= BGFX_STENCIL_OP_FAIL_Z_INVERT;	break;
			}
			switch (_stencil.pass)
			{
				case StencilOperation::Zero:				result |= BGFX_STENCIL_OP_PASS_Z_ZERO;		break;
				case StencilOperation::Keep:				result |= BGFX_STENCIL_OP_PASS_Z_KEEP;		break;
				case StencilOperation::Replace:				result |= BGFX_STENCIL_OP_PASS_Z_REPLACE;	break;
				case StencilOperation::Increment:			result |= BGFX_STENCIL_OP_PASS_Z_INCR;		break;
				case StencilOperation::IncrementSaturate:	result |= BGFX_STENCIL_OP_PASS_Z_INCRSAT;	break;
				case StencilOperation::Decrement:			result |= BGFX_STENCIL_OP_PASS_Z_DECR;		break;
				case StencilOperation::DecrementSaturate:	result |= BGFX_STENCIL_OP_PASS_Z_DECRSAT;	break;
				case StencilOperation::Invert:				result |= BGFX_STENCIL_OP_PASS_Z_INVERT;	break;
			}
			return result;
		}
		uint32_t getStencilState(const Stencil* _stencil)
		{
			if (!_stencil)
			{
				return BGFX_STENCIL_DEFAULT;
			}

			se_assert(_stencil->refValue <= 0xffu);
			se_assert(_stencil->maskValue <= 0xffu);
			return BGFX_STENCIL_NONE
				| BGFX_STENCIL_FUNC_REF(_stencil->refValue)
				| BGFX_STENCIL_FUNC_RMASK(_stencil->maskValue)
				| setStencilTestState(*_stencil)
				| setStencilOperationsState(*_stencil)
				;
		}
	}
}
