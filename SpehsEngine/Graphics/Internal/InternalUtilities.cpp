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

			if (checkBit(_renderInfo.renderFlags, RenderFlag::DepthTest))
				state |= BGFX_STATE_DEPTH_TEST_LESS; // TODO

			if (checkBit(_renderContext.rendererFlags, RendererFlag::CWFrontFace))
				state |= BGFX_STATE_FRONT_CCW;
			se_assert(!(checkBit(_renderInfo.renderFlags, RenderFlag::CullBackFace) && checkBit(_renderInfo.renderFlags, RenderFlag::CullFrontFace)));
			if (checkBit(_renderInfo.renderFlags, RenderFlag::CullBackFace))
				state |= checkBit(_renderContext.rendererFlags, RendererFlag::CWFrontFace) ? BGFX_STATE_CULL_CCW : BGFX_STATE_CULL_CW;
			else if (checkBit(_renderInfo.renderFlags, RenderFlag::CullFrontFace))
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

			// TODO: BGFX_STATE_BLEND*
			// TODO: BGFX_STATE_POINT_SIZE
			// TODO: BGFX_STATE_LINEAA
			// TODO: BGFX_STATE_CONSERVATIVE_RASTER

			bgfx::setState(state);
		}
	}
}
