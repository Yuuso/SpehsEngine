#include "stdafx.h"
#include "SpehsEngine/GUI/Internal/GUIMaterialManager.h"


namespace se
{
	namespace gui
	{
		GUIMaterialManager::GUIMaterialManager(graphics::ShaderManager& _shaderManager, graphics::TextureManager& _textureManager, graphics::FontManager& _fontManager)
			: shaderManager(_shaderManager)
			, textureManager(_textureManager)
			, fontManager(_fontManager)
		{
			graphics::TextureInput textureInput;
			textureInput.width = 1;
			textureInput.height = 1;
			textureInput.data = { 255, 255, 255, 255 };
			colorTexture = textureManager.create("SE_GUI_WhiteColor", textureInput);
		}

		std::shared_ptr<graphics::Texture> GUIMaterialManager::getTexture(std::string_view _texture)
		{
			if (_texture.empty())
				return colorTexture;
			auto texture = textureManager.find(_texture);
			if (!texture)
				texture = textureManager.create(_texture, _texture, textureModes);
			return texture;
		}
		std::shared_ptr<graphics::Font> GUIMaterialManager::getFont(std::string_view _font)
		{
			if (_font.empty())
				return fontManager.getDefaultFont();
			auto font = fontManager.find(_font);
			if (!font)
				font = fontManager.create(_font, _font, graphics::FontSize());
			font->waitUntilReady();
			return font;
		}

		std::shared_ptr<graphics::Material> GUIMaterialManager::createShapeMaterial(std::string_view _texture)
		{
			auto result = graphics::createMaterial(graphics::DefaultMaterialType::FlatTexture, shaderManager);
			result->setTexture(getTexture(_texture));
			return result;
		}
		std::shared_ptr<graphics::Material> GUIMaterialManager::createFontMaterial(std::string_view _font)
		{
			auto result = graphics::createMaterial(graphics::DefaultMaterialType::Text, shaderManager);
			result->setFont(getFont(_font));
			return result;
		}

		void GUIMaterialManager::setTextureModes(const graphics::TextureModes& _textureModes)
		{
			textureModes = _textureModes;
		}
	}
}
