#pragma once

#include "glm/mat4x4.hpp"
#include "SpehsEngine/GUI/Types.h"
#include <numeric>


namespace se
{
	namespace gui
	{
		class GUIView;

		enum GUIElementUpdateFlag : GUIElementUpdateFlagsType
		{
			PrimitiveAddNeeded		= (1 << 0),
			TreeUpdateNeeded		= (1 << 1),
			MaterialUpdateNeeded	= (1 << 2),
			EverythingChanged		= std::numeric_limits<GUIElementUpdateFlagsType>::max()
		};

		struct UpdateContext
		{
			UpdateContext(GUIView& _view)
				: view(_view) {}

			GUIView& view;
		};
	}
}
