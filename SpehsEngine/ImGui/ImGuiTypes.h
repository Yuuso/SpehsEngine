#pragma once

#include "SpehsEngine/Graphics/ResourceHandle.h"


namespace se
{
	namespace imgui
	{
		enum ImGuiFont
		{
			Mono,
			Regular,
			Count
		};

		union ImGuiUserTextureData
		{
			void* id;
			graphics::ResourceHandle resourceHandle;
		};
	}
}
