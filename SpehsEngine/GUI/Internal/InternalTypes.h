#pragma once

#include "glm/mat4x4.hpp"
#include "SpehsEngine/GUI/Types.h"


namespace se
{
	namespace gfx
	{
		class Scene;
	}
	namespace input
	{
		struct MouseButtonEvent;
	}
	namespace gui
	{
		class StencilMaskManager;
	}

	namespace gui
	{
		class GUIMaterialManager;


		enum GUIElementUpdateFlag : GUIElementUpdateFlagsType
		{
			PrimitiveAddNeeded		= (1 << 0),
			TreeUpdateNeeded		= (1 << 1),
			MaterialUpdateNeeded	= (1 << 2),
			VisualUpdateNeeded		= (1 << 3),
			EverythingChanged		= std::numeric_limits<GUIElementUpdateFlagsType>::max()
		};

		struct UpdateContext
		{
			gfx::Scene& scene;
			GUIMaterialManager& materialManager;
			gui::StencilMaskManager & stencilMaskManager;

			glm::vec2 viewSize;
			ZIndex hoverHandledDepth = std::numeric_limits<ZIndex>::min();
		};

		struct InputUpdateContext
		{
			const input::MouseButtonEvent& mouseButtonEvent;
			ZIndex handledDepth = std::numeric_limits<ZIndex>::min();
		};

		enum class GUIElementInputStatus
		{
			Normal,
			Hover,
			Pressed
		};

	#define return_property(__p) \
		switch (inputStatus) \
		{ \
			case GUIElementInputStatus::Pressed: \
				if (pressedProperties.has_value() && pressedProperties->__p.has_value()) \
					return pressedProperties->__p.value(); \
				[[fallthrough]]; \
			case GUIElementInputStatus::Hover: \
				if (hoverProperties.has_value() && hoverProperties->__p.has_value()) \
					return hoverProperties->__p.value(); \
				[[fallthrough]]; \
			case GUIElementInputStatus::Normal: \
				se_assert(normalProperties.__p.has_value()); \
				return normalProperties.__p.value(); \
		} \
		se_assert_m(false, "Invalid input status!"); \
		se_assert(normalProperties.__p.has_value()); \
		return normalProperties.__p.value()
	}
}
