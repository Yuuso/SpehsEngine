#include "stdafx.h"
#include "SpehsEngine/Graphics/TextureInput.h"


namespace se::gfx
{
	#define CHECK_NULL_DATA \
	if (!data) \
	{ \
		se_assert(behavior == TextureInputResetBehavior::Nothing); \
		log::error("TextureInput data is null! Can't use TextureInput more than once with TextureInputResetBehavior::Nothing."); \
		return; \
	}
	#define CHECK_DATA_FORMAT(FORMAT) \
	if (data->format != FORMAT) \
	{ \
		log::error("Invalid TextureInput format for given input."); \
		return; \
	}

	TextureInput::TextureInput(
		uint16_t _width,
		uint16_t _height,
		TextureFormat _format,
		bool _isCubemap,
		TextureInputResetBehavior _behavior)
	{
		se_assert(_width > 0);
		se_assert(_height > 0);
		behavior = _behavior;
		init(
			std::max(_width, uint16_t{1}),
			std::max(_height, uint16_t{1}),
			_format,
			_isCubemap);
	}
	void TextureInput::push(float _value, std::optional<size_t> _pixelIndex)
	{
		CHECK_NULL_DATA;
		CHECK_DATA_FORMAT(TextureFormat::R8);
		se_assert(_value >= 0.0f && _value <= 1.0f);
		data->data[getDataIndex(_pixelIndex)] = static_cast<uint8_t>(_value * UINT8_MAX);
	}
	void TextureInput::push(uint8_t _value, std::optional<size_t> _pixelIndex)
	{
		CHECK_NULL_DATA;
		CHECK_DATA_FORMAT(TextureFormat::R8);
		data->data[getDataIndex(_pixelIndex)] = _value;
	}
	void TextureInput::push(const glm::vec4& _value, std::optional<size_t> _pixelIndex)
	{
		CHECK_NULL_DATA;
		CHECK_DATA_FORMAT(TextureFormat::RGBA8);
		const size_t index = getDataIndex(_pixelIndex);
		data->data[index + 0] = static_cast<uint8_t>(_value.r * UINT8_MAX);
		data->data[index + 1] = static_cast<uint8_t>(_value.g * UINT8_MAX);
		data->data[index + 2] = static_cast<uint8_t>(_value.b * UINT8_MAX);
		data->data[index + 3] = static_cast<uint8_t>(_value.a * UINT8_MAX);
	}
	void TextureInput::push(const Color& _value, std::optional<size_t> _pixelIndex)
	{
		CHECK_NULL_DATA;
		CHECK_DATA_FORMAT(TextureFormat::RGBA8);
		const size_t index = getDataIndex(_pixelIndex);
		data->data[index + 0] = static_cast<uint8_t>(_value.r * UINT8_MAX);
		data->data[index + 1] = static_cast<uint8_t>(_value.g * UINT8_MAX);
		data->data[index + 2] = static_cast<uint8_t>(_value.b * UINT8_MAX);
		data->data[index + 3] = static_cast<uint8_t>(_value.a * UINT8_MAX);
	}
	void TextureInput::push(uint8_t r, uint8_t g, uint8_t b, uint8_t a, std::optional<size_t> _pixelIndex)
	{
		CHECK_NULL_DATA;
		CHECK_DATA_FORMAT(TextureFormat::RGBA8);
		const size_t index = getDataIndex(_pixelIndex);
		data->data[index + 0] = r;
		data->data[index + 1] = g;
		data->data[index + 2] = b;
		data->data[index + 3] = a;
	}
	void TextureInput::setData(const std::vector<uint8_t>& _data)
	{
		CHECK_NULL_DATA;
		data->data = _data;
	}
	std::shared_ptr<TextureInputData> TextureInput::collectData()
	{
		if (!data)
		{
			se_assert(behavior == TextureInputResetBehavior::Nothing);
			log::error("TextureInput data is null! Can't use TextureInput more than once with TextureInputResetBehavior::Nothing.");
			return nullptr;
		}
		std::shared_ptr<TextureInputData> result = data;
		switch (behavior)
		{
			case TextureInputResetBehavior::Nothing:
				data.reset();
				break;
			case TextureInputResetBehavior::Clear:
				init(result->width, result->height, result->format, result->isCubemap);
				break;
			case TextureInputResetBehavior::Copy:
				init(result->width, result->height, result->format, result->isCubemap);
				data->data = result->data;
				break;
		}
		return result;
	}
	void TextureInput::init(uint16_t _width, uint16_t _height, TextureFormat _format, bool _isCubemap)
	{
		se_assert(_format == TextureFormat::RGBA8 || _format == TextureFormat::R8);
		data = std::make_shared<TextureInputData>();
		data->width = _width;
		data->height = _height;
		data->format = _format;
		data->isCubemap = _isCubemap;
		data->data.resize(size_t(_width * _height)
			* size_t{_format == TextureFormat::RGBA8 ? 4u : 1u}
			* size_t{_isCubemap ? 6u : 1u});
	}
	size_t TextureInput::getDataIndex(std::optional<size_t> _pixelIndex)
	{
		if (!_pixelIndex.has_value())
		{
			_pixelIndex = nextPixelIndex;
		}
		nextPixelIndex = _pixelIndex.value() + 1;
		size_t dataIndex = _pixelIndex.value()
			* size_t{ data->format == TextureFormat::RGBA8 ? 4u : 1u };
		se_assert(dataIndex < data->data.size());
		return std::clamp(dataIndex, size_t{ 0 }, data->data.size());
	}
}
