#pragma once

namespace se::gfx
{
	struct TextureData;
}

namespace se::gfx::impl
{
	struct TextureFallbacks
	{
		std::shared_ptr<TextureData> init;
		std::shared_ptr<TextureData> loading;
		std::shared_ptr<TextureData> error;
	};
}
