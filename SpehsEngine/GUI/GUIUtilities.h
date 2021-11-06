#pragma once

#include "SpehsEngine/GUI/Types.h"


namespace se
{
	namespace gui
	{
		// TODO: Move away from here I think....
		inline GUIUnit alignmentToUnit(VerticalAlignment _alignment)
		{
			switch (_alignment)
			{
				case VerticalAlignment::Top:		return GUIUnit(0.0f, GUIUnitType::Self);
				case VerticalAlignment::Middle:		return GUIUnit(0.5f, GUIUnitType::Self);
				case VerticalAlignment::Bottom:		return GUIUnit(1.0f, GUIUnitType::Self);
			}
			se_assert(false);
			return GUIUnit{};
		}
		inline GUIUnit alignmentToUnit(HorizontalAlignment _alignment)
		{
			switch (_alignment)
			{
				case HorizontalAlignment::Left:		return GUIUnit(0.0f, GUIUnitType::Self);
				case HorizontalAlignment::Center:	return GUIUnit(0.5f, GUIUnitType::Self);
				case HorizontalAlignment::Right:	return GUIUnit(1.0f, GUIUnitType::Self);
			}
			se_assert(false);
			return GUIUnit{};
		}
	}
}
