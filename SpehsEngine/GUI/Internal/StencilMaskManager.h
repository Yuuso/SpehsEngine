#pragma once

#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/GUI/Types.h"
#include <memory>


namespace se::gui
{
	enum class LayerMaskStyle
	{
		/*
		* Elements are masked with a global layer counter.
		* This means that you can use almost as many layer masked elements as you want.
		* Masked elements can bleed to other nearby masked elements that are on the same layer.
		* There is a max limit of 255 nested masked elements.
		*
		* Use this only if you have a ton of masked elements and the limit from 'Incrementing' style is not enough.
		*/
		Depth,

		/*
		* Similar to 'Depth', but elements without a masking parent start from a new layer.
		* Child elements who have the same masking parent however still share the same masking layer.
		* There is a limit of 255 total unique masked elements.
		*
		* Use this as a default.
		*/
		Incrementing,

		/*
		* Masked elements reserve one bit for masking.
		* This means there is never bleeding to neighboring elements.
		* But masked elements are limited to only 8!
		*
		* Use this if you don't need more than 8 total masked elements in the view.
		*/
		BitMask,
	};

	// Interface
	class StencilMaskManager
	{
	public:
		virtual ~StencilMaskManager() = default;
		virtual void begin() = 0;
		virtual void end() = 0;
		virtual bool enabled() const = 0;
		virtual gfx::Stencil get(bool _write) const = 0;
	};

	std::unique_ptr<StencilMaskManager> createStencilMaskManager(LayerMaskStyle _style);
}
