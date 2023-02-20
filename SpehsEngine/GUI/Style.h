#pragma once

#include "stdafx.h"
#include "SpehsEngine/GUI/Resource.h"
#include "SpehsEngine/GUI/PropertySetter.h"
#include "SpehsEngine/GUI/PropertyTrigger.h"
#include "SpehsEngine/GUI/EventTrigger.h"


namespace se::gui
{
	class Style
	{
	public:
		std::optional<ResourceKey> basedOn;
		std::vector<PropertySetter> setters;
		std::vector<PropertyTrigger> propertyTriggers;
		std::vector<EventTrigger> eventTriggers;
	};
}
