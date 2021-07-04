#pragma once

#include "SpehsEngine/Graphics/Internal/ResourceHandle.h"


namespace se
{
	namespace imgui
	{
		enum ImGuiFont
		{
			Regular,
			Mono,
			Count
		};

		union ImGuiUserTextureData
		{
			void* id;
			graphics::ResourceHandle resourceHandle;
		};
	}
}
