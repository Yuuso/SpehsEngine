#pragma once

#include <stdint.h>


namespace se::gui
{
	enum class PropertyValueType : uint16_t
	{
		Default,
		ImplicitResource,
		Style,
		BindingSource,
		Local,
		ExplicitResource = Local,
		BindingTarget = Local,
	};
}
