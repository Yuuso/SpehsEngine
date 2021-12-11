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

		std::shared_ptr<graphics::Material> GUIMaterialManager::createColorMaterial()
		{
			auto result = graphics::createMaterial(graphics::DefaultMaterialType::FlatTexture, shaderManager);
			result->setTexture(colorTexture);
			return result;

			// TODO: Figure out blending issues, we're using the same shader for all GUIShapes for now...
			//			One possible fix would be to use Sequential view ordering only for GUI views.
			//return graphics::createMaterial(graphics::DefaultMaterialType::FlatColor, shaderManager);
		}
		std::shared_ptr<graphics::Material> GUIMaterialManager::createTextureMaterial(std::string_view _texture)
		{
			auto result = graphics::createMaterial(graphics::DefaultMaterialType::FlatTexture, shaderManager);
			auto texture = textureManager.find(_texture);
			if (!texture)
				texture = textureManager.create(_texture, _texture);
			result->setTexture(texture);
			return result;
		}
		std::shared_ptr<graphics::Material> GUIMaterialManager::createFontMaterial(std::string_view _font)
		{
			auto result = graphics::createMaterial(graphics::DefaultMaterialType::Text, shaderManager);
			auto font = _font.empty() ? fontManager.getDefaultFont() : fontManager.find(_font);
			if (!font)
				font = fontManager.create(_font, _font, graphics::FontSize());
			font->waitUntilReady();
			result->setFont(font);
			return result;
		}
	}
}
