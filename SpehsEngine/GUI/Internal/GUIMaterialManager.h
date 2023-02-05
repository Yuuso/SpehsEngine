#pragma once

#include "SpehsEngine/Graphics/ShaderManager.h"
#include "SpehsEngine/Graphics/TextureManager.h"
#include "SpehsEngine/Graphics/FontManager.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"
#include "SpehsEngine/Graphics/ShapeGenerator.h"


namespace se
{
	namespace gui
	{
		// TODO...
		class GUIMaterialManager
		{
		public:
			GUIMaterialManager(graphics::ShaderManager& _shaderManager, graphics::TextureManager& _textureManager, graphics::FontManager& _fontManager);

			std::shared_ptr<graphics::Texture> getTexture(std::string_view _texture);
			std::shared_ptr<graphics::Font> getFont(std::string_view _font);

			std::shared_ptr<graphics::Material> createShapeMaterial(std::string_view _texture);
			std::shared_ptr<graphics::Material> createFontMaterial(std::string_view _font);

			graphics::ShapeGenerator shapeGenerator;

		private:

			std::shared_ptr<graphics::Texture> colorTexture;

			graphics::ShaderManager& shaderManager;
			graphics::TextureManager& textureManager;
			graphics::FontManager& fontManager;
		};
	}
}
