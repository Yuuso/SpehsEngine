#pragma once

#include "SpehsEngine/Core/AssetManager.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"
#include "SpehsEngine/Graphics/ShapeGenerator.h"
#include "SpehsEngine/Graphics/Texture.h"


namespace se
{
	namespace gui
	{
		// TODO...
		class GUIMaterialManager
		{
		public:
			GUIMaterialManager(AssetManager& _assetManager);

			std::shared_ptr<gfx::Texture> getTexture(std::string_view _texture);
			std::shared_ptr<gfx::Font> getFont(std::string_view _font);

			std::shared_ptr<gfx::Material> createShapeMaterial(std::string_view _texture);
			std::shared_ptr<gfx::Material> createFontMaterial(std::string_view _font);

			gfx::ShapeGenerator shapeGenerator;

		private:

			std::shared_ptr<gfx::Texture> colorTexture;

			AssetManager& assetManager;
		};
	}
}
