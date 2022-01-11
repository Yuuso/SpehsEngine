#pragma once

#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/GUI/Types.h"
#include "SpehsEngine/GUI/Units.h"
#include "glm/vec2.hpp"
#include <optional>


namespace se
{
	namespace gui
	{
		struct GUIElementProperties
		{
			// GUIElement
			std::optional<GUIVec2>			position;
			std::optional<ZIndex>			zindex;
			std::optional<float>			rotation;
			std::optional<GUIVec2>			size;
			std::optional<glm::vec2>		scale;
			std::optional<bool>				clipping;
			std::optional<GUIVec2>			anchor;
			std::optional<GUIVec2>			alignment;
			std::optional<GUIUnit>			padding;
			std::optional<bool>				visible;

			// GUIShape
			std::optional<Color>			color;
			std::optional<std::string>		texture;

			// GUIText
			std::optional<std::string>		font;
		};

		static const GUIElementProperties defaultGUIElementProperties = {
			/* position	 */ GUIVec2({ 0.0f, 0.0f }, GUIUnitType::Auto),
			/* zindex	 */ ZIndex(0),
			/* rotation	 */ 0.0f,
			/* size		 */ GUIVec2({ 0.0f, 0.0f }, GUIUnitType::Auto),
			/* scale	 */ glm::vec2{ 1.0f, 1.0f },
			/* clipping	 */ false,
			/* anchor	 */ GUIVec2({ 0.0f, 0.0f }, GUIUnitType::Self),
			/* alignment */ GUIVec2({ 0.0f, 0.0f }, GUIUnitType::Parent),
			/* padding	 */ GUIUnit(0.0f, GUIUnitType::Pixels),
			/* visible	 */ true,

			/* color	 */ Color(),
			/* texture	 */ "",

			/* font		 */ "",
		};
	}
}
