#pragma once

#include "SpehsEngine/Graphics/ShaderManager.h"
#include "SpehsEngine/Graphics/TextureManager.h"
#include "SpehsEngine/Graphics/FontManager.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"
#include "SpehsEngine/Graphics/ShapeGenerator.h"


namespace se
{
	namespace legacygui
	{
		// TODO...
		class GUIMaterialManager
		{
		public:
			GUIMaterialManager(gfx::ShaderManager& _shaderManager, gfx::TextureManager& _textureManager, gfx::FontManager& _fontManager);

			std::shared_ptr<gfx::Texture> getTexture(std::string_view _texture);
			std::shared_ptr<gfx::Font> getFont(std::string_view _font);

			std::shared_ptr<gfx::Material> createShapeMaterial(std::string_view _texture);
			std::shared_ptr<gfx::Material> createFontMaterial(std::string_view _font);

			gfx::ShapeGenerator shapeGenerator;

		private:

			std::shared_ptr<gfx::Texture> colorTexture;

			gfx::ShaderManager& shaderManager;
			gfx::TextureManager& textureManager;
			gfx::FontManager& fontManager;
		};
	}
}
