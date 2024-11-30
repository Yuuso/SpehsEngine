#include "stdafx.h"
#include "SpehsEngine/GUI/Internal/GUIMaterialManager.h"

#include "SpehsEngine/Graphics/DefaultFonts.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"


namespace se
{
	namespace gui
	{
		GUIMaterialManager::GUIMaterialManager(AssetManager& _assetManager)
			: assetManager(_assetManager)
		{
			gfx::TextureInput textureInput{ 1, 1 };
			textureInput.push(255, 255, 255, 255);
			colorTexture = std::make_shared<gfx::Texture>("SE_GUI_WhiteColor", nullptr, textureInput);
		}

		std::shared_ptr<gfx::Texture> GUIMaterialManager::getTexture(std::string_view _texture)
		{
			if (_texture.empty())
				return colorTexture;
			auto texture = assetManager.find<gfx::Texture>(_texture);
			if (!texture)
			{
				texture = assetManager.emplace<gfx::Texture>(_texture, _texture, textureModes);
			}
			return texture;
		}
		std::shared_ptr<gfx::Font> GUIMaterialManager::getFont(std::string_view _font)
		{
			if (_font.empty())
				return assetManager.find<gfx::Font>(gfx::getDefaultFontName());
			auto font = assetManager.find<gfx::Font>(_font);
			if (!font)
			{
				font = assetManager.emplace<gfx::Font>(_font, _font);
			}
			font->waitUntilReady();
			return font;
		}

		std::shared_ptr<gfx::Material> GUIMaterialManager::createShapeMaterial(std::string_view _texture)
		{
			auto result = gfx::createMaterial(gfx::DefaultMaterialType::FlatTexture, assetManager);
			result->setTexture(getTexture(_texture));
			return result;
		}
		std::shared_ptr<gfx::Material> GUIMaterialManager::createFontMaterial(std::string_view _font)
		{
			auto result = gfx::createMaterial(gfx::DefaultMaterialType::Text, assetManager);
			result->setFont(getFont(_font));
			return result;
		}

		void GUIMaterialManager::setTextureModes(const gfx::TextureModes& _textureModes)
		{
			textureModes = _textureModes;
		}
	}
}
