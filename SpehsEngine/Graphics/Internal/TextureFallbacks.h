#pragma once


namespace se
{
	namespace graphics
	{
		struct TextureData;

		struct TextureFallbacks
		{
			std::shared_ptr<TextureData> init;
			std::shared_ptr<TextureData> loading;
			std::shared_ptr<TextureData> error;
		};
	}
}
