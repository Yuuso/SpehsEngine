#pragma once


namespace se::gfx
{
	enum class TextureFormat
	{
		RGBA8,
		R8,
	};

	// Because of async loading, data needs to be consumed by async thread.
	// TextureInputResetBehavior tells what to do on TextureInput::getData().
	enum class TextureInputResetBehavior
	{
		// Leaves data null.
		// TextureInput won't be reusable.
		Nothing,

		// Reinitializes TextureInputData.
		// Use if you want to create multiple similar textures.
		Clear,

		// Leaves data untouched and copies it to async thread instead.
		// Use if you need to use the data after starting to load.
		// (e.g. Creating multiple identical textures).
		Copy,

		Default = Nothing,
	};

	struct TextureInputData
	{
		std::vector<uint8_t> data;
		TextureFormat format = TextureFormat::RGBA8;
		uint16_t width = 0;
		uint16_t height = 0;
		bool isCubemap = false;
	};

	class TextureInput
	{
	public:

		TextureInput(
			uint16_t _width,
			uint16_t _height,
			TextureFormat _format = TextureFormat::RGBA8,
			bool _isCubemap = false,
			TextureInputResetBehavior _behavior = TextureInputResetBehavior::Default);

		//
		// push method params:
		//	uint8_t values in range 0 - 255.
		//	float/color values in normalized range 0.0 - 1.0.
		//
		//	index is index of the pixel, not the index in data array.
		//	nullopt index pushes to the next index from previous push, starting from 0.
		//

		// Push R8 value
		void push(float _value, std::optional<size_t> _pixelIndex = std::nullopt);
		void push(uint8_t _value, std::optional<size_t> _pixelIndex = std::nullopt);

		// Push RGBA8 value
		void push(const glm::vec4& _value, std::optional<size_t> _pixelIndex = std::nullopt);
		void push(const Color& _value, std::optional<size_t> _pixelIndex = std::nullopt);
		void push(uint8_t r, uint8_t g, uint8_t b, uint8_t a, std::optional<size_t> _pixelIndex = std::nullopt);

		// Replace data by copy
		void setData(const std::vector<uint8_t> & _data);

		// Returns current data and resets.
		// This is called by Texture when async loading.
		// see TextureInputResetBehavior.
		std::shared_ptr<TextureInputData> collectData();

	private:

		void init(uint16_t _width, uint16_t _height, TextureFormat _format, bool _isCubemap);
		size_t getDataIndex(std::optional<size_t> _pixelIndex);

		std::shared_ptr<TextureInputData> data;
		TextureInputResetBehavior behavior = TextureInputResetBehavior::Default;
		size_t nextPixelIndex = 0;
	};

	// How the cube map data is structured (bgfx docs have faces 2 & 3 swapped?):
	//  +x, -x, +y, -y, +z, -z
	//
	//                  +----------+
	//                  |-z       3|
	//                  | ^  +y    |
	//                  | |        |    Unfolded cube:
	//                  | +---->+x |
	//       +----------+----------+----------+----------+
	//       |+y       1|+y       4|+y       0|+y       5|
	//       | ^  -x    | ^  +z    | ^  +x    | ^  -z    |
	//       | |        | |        | |        | |        |
	//       | +---->+z | +---->+x | +---->-z | +---->-x |
	//       +----------+----------+----------+----------+
	//                  |+z       2|
	//                  | ^  -y    |
	//                  | |        |
	//                  | +---->+x |
	//                  +----------+
	//
}
