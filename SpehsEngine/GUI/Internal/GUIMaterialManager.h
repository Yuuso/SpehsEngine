#pragma once

#include "SpehsEngine/Graphics/ShaderManager.h"
#include "SpehsEngine/Graphics/TextureManager.h"
#include "SpehsEngine/Graphics/FontManager.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"


namespace se
{
	namespace gui
	{
		// TODO...
		class GUIMaterialManager
		{
		public:
			GUIMaterialManager(graphics::ShaderManager& _shaderManager, graphics::TextureManager& _textureManager, graphics::FontManager& _fontManager);

			std::shared_ptr<graphics::Material> createColorMaterial();
			std::shared_ptr<graphics::Material> createTextureMaterial(std::string_view _texture);
			std::shared_ptr<graphics::Material> createFontMaterial(std::string_view _font);

		private:

			graphics::ShaderManager& shaderManager;
			graphics::TextureManager& textureManager;
			graphics::FontManager& fontManager;
		};
	}
}
