#pragma once

#include "SpehsEngine/GUI/Types.h"


namespace se
{
	namespace legacygui
	{
		GUIUnit anchorToUnit(VerticalAlignment _alignment);
		GUIUnit anchorToUnit(HorizontalAlignment _alignment);

		GUIUnit alignmentToUnit(VerticalAlignment _alignment);
		GUIUnit alignmentToUnit(HorizontalAlignment _alignment);
	}
}
