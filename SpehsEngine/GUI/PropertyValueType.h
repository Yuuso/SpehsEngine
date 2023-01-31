#pragma once

#include <stdint.h>


namespace se::gui
{
	enum class PropertyValueType : uint16_t
	{
		Default,
		Style,
		Resource,
		Local,
	};
}
