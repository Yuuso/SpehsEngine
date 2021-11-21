#include "stdafx.h"
#include "SpehsEngine/GUI/GUIUtilities.h"


namespace se
{
	namespace gui
	{
		GUIUnit anchorToUnit(VerticalAlignment _alignment)
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
		GUIUnit anchorToUnit(HorizontalAlignment _alignment)
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

		GUIUnit alignmentToUnit(VerticalAlignment _alignment)
		{
			switch (_alignment)
			{
				case VerticalAlignment::Top:		return GUIUnit(0.0f, GUIUnitType::Parent);
				case VerticalAlignment::Middle:		return GUIUnit(0.5f, GUIUnitType::Parent);
				case VerticalAlignment::Bottom:		return GUIUnit(1.0f, GUIUnitType::Parent);
			}
			se_assert(false);
			return GUIUnit{};
		}
		GUIUnit alignmentToUnit(HorizontalAlignment _alignment)
		{
			switch (_alignment)
			{
				case HorizontalAlignment::Left:		return GUIUnit(0.0f, GUIUnitType::Parent);
				case HorizontalAlignment::Center:	return GUIUnit(0.5f, GUIUnitType::Parent);
				case HorizontalAlignment::Right:	return GUIUnit(1.0f, GUIUnitType::Parent);
			}
			se_assert(false);
			return GUIUnit{};
		}
	}
}
