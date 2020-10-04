#pragma once

#include "SpehsEngine/Graphics/ResourceData.h"
#include <memory>


namespace se
{
	namespace graphics
	{
		struct TextureFallbacks
		{
			std::shared_ptr<TextureData> init;
			std::shared_ptr<TextureData> loading;
			std::shared_ptr<TextureData> error;
		};
	}
}
