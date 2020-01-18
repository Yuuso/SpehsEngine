#pragma once

#include "bgfx/bgfx.h"
#include "SpehsEngine/Graphics/Internal/InternalTypes.h"
#include "SpehsEngine/Graphics/Types.h"


namespace se
{
	namespace graphics
	{
		void applyRenderState(const RenderInfo& _renderInfo, const RenderContext& _renderContext);
		const bgfx::VertexLayout findVertexLayout(const VertexAttributeFlagsType _attributes); // TODO: Cache this too?
	}
}
