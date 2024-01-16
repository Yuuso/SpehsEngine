#pragma once

#include "SpehsEngine/Graphics/VertexAttribute.h"
#include "SpehsEngine/Core/Asset.h"


namespace se::gfx::impl
{
	struct RenderInfo;

	void applyRenderState(const RenderInfo& _renderInfo, const RenderContext& _renderContext);
	const bgfx::VertexLayout findVertexLayout(const VertexAttributeFlagsType _attributes); // TODO: Cache this too?
	uint64_t TextureModesToFlags(const TextureModes _textureModes);
	uint32_t getResetParameters(const RendererFlagsType _rendererFlags);
	bgfx::RendererType::Enum getRendererType(const RendererBackend _rendererBackend);
	RendererBackend getRendererBackend();
	bgfx::UniformType::Enum getUniformType(const UniformType _type);
	bgfx::ViewMode::Enum getViewMode(DrawCallSortOrder _type);
	uint32_t getStencilState(const Stencil* _stencil);

	template <typename T>
	void safeDestroy(AssetHandle& _assetHandle)
	{
		static_assert(sizeof(T) == sizeof(_assetHandle.u16handle));
		T bgfxHandle = T{ _assetHandle.u16handle };
		if (bgfx::isValid(bgfxHandle))
		{
			bgfx::destroy(bgfxHandle);
		}
		_assetHandle.u16handle = bgfx::kInvalidHandle;
	}
}
