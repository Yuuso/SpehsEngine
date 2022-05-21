#pragma once

#include <stdint.h>
#include <vector>


namespace se
{
	namespace graphics
	{
		struct TextureInput
		{
			enum class Format
			{
				RGBA8,
				R8,
			};

			bool isCubemap = false; // +x, -x, +y, -y, +z, -z
			Format format = Format::RGBA8;
			uint16_t width = 0;
			uint16_t height = 0;
			std::vector<uint8_t> data;
		};
	}
}
