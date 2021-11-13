#pragma once

#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "SpehsEngine/GUI/Types.h"
#include <numeric>


namespace se
{
	namespace graphics
	{
		class Scene;
	}

	namespace gui
	{
		class GUIMaterialManager;

		enum GUIElementUpdateFlag : GUIElementUpdateFlagsType
		{
			PrimitiveAddNeeded		= (1 << 0),
			TreeUpdateNeeded		= (1 << 1),
			MaterialUpdateNeeded	= (1 << 2),
			EverythingChanged		= std::numeric_limits<GUIElementUpdateFlagsType>::max()
		};

		struct UpdateContext
		{
			graphics::Scene& scene;
			GUIMaterialManager& materialManager;

			glm::vec2 viewSize;
		};
	}
}
