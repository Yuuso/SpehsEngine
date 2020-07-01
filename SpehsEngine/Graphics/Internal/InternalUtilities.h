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
		uint64_t TextureModesToFlags(const TextureModes _textureModes);
		uint32_t getResetParameters(const RendererFlagsType _rendererFlags);
		bgfx::RendererType::Enum getRendererType(const RendererBackend _rendererBackend);
		RendererBackend getRendererBackend();
	}
}
