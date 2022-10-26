#pragma once

#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/GUI/Types.h"
#include <memory>


namespace se::gui
{
	// Interface
	class StencilMaskManager
	{
	public:
		virtual ~StencilMaskManager() = default;
		virtual void begin() = 0;
		virtual void end() = 0;
		virtual bool enabled() const = 0;
		virtual graphics::Stencil get(bool _write) const = 0;
	};

	std::unique_ptr<StencilMaskManager> createStencilMaskManager(LayerMaskStyle _style);
}
