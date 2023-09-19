#pragma once

#include <stdint.h>


namespace se::gui
{
	enum class PropertyPrecedence : uint16_t
	{
		Default,
		ImplicitBaseStyle,
		ImplicitStyle,
		ImplicitResource,
		BaseStyle,
		Style,
		BindingSource,
		Local,
		ExplicitResource = Local,
		BindingTarget = Local
	};

	// NOTE: Triggers and Animation for each precedence level will override the value.
}
