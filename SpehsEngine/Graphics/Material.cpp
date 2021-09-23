#include "stdafx.h"
#include "SpehsEngine/Graphics/Material.h"


namespace se
{
	namespace graphics
	{
		void Material::bind()
		{
			for (auto&& font : fonts)
			{
				se_assert(font.second);
				if (font.second)
					font.second->uniform->set(*font.second->font.get(), font.first);
			}
			for (auto&& texture : textures)
			{
				se_assert(texture.second);
				if (texture.second)
					texture.second->uniform->set(*texture.second->texture.get(), texture.first);
			}
			for (auto&& uniformContainer : uniformContainers)
			{
				se_assert(uniformContainer);
				if (uniformContainer)
					uniformContainer->bind();
			}
		}
		void Material::addUniformContainer(std::shared_ptr<UniformContainer> _uniformContainer)
		{
			uniformContainers.push_back(_uniformContainer);
		}
		void Material::connectToFontChangedSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<void(void)>& callback)
		{
			scopedConnection = fontChangedSignal.connect(callback);
		}

		std::shared_ptr<Shader> Material::getShader(const ShaderVariant variant) const
		{
			auto it = shaders.find(variant);
			if (it != shaders.end())
				return it->second;

			log::warning("Material is missing " + std::to_string(static_cast<int>(variant)) + " shader variant!");
			it = shaders.find(ShaderVariant::Default);
			if (it != shaders.end())
				return it->second;

			log::warning("Material is also missing the default variant!");
			return nullptr;
		}
		std::shared_ptr<Font> Material::getFont(const uint8_t _slot) const
		{
			auto it = fonts.find(_slot);
			if (it == fonts.end())
				return nullptr;
			return it->second->font;
		}
		std::shared_ptr<Texture> Material::getTexture(const uint8_t _slot) const
		{
			auto it = textures.find(_slot);
			if (it == textures.end())
				return nullptr;
			return it->second->texture;
		}
		bool Material::getLit() const
		{
			return lightsEnabled;
		}

		void Material::setShader(std::shared_ptr<Shader> _shader, const ShaderVariant variant)
		{
			shaders[variant] = _shader;
		}
		void Material::setTexture(std::shared_ptr<Texture> _texture, const std::string_view _uniformName, const uint8_t _slot)
		{
			if (_uniformName.empty())
			{
				log::error("Empty material texture uniform name!");
				return;
			}
			std::unique_ptr<MaterialTexture>& materialTexture = textures[_slot];
			if (!materialTexture)
			{
				materialTexture = std::make_unique<MaterialTexture>();
				materialTexture->uniform = std::make_unique<Uniform>(_uniformName, UniformType::Sampler);
			}
			materialTexture->texture = _texture;
		}
		void Material::setTexture(std::shared_ptr<Texture> _texture, const uint8_t _slot)
		{
			auto it = textures.find(_slot);
			if (it == textures.end())
			{
				log::error("Texture in slot " + std::to_string(_slot) + " doesn't exist!");
				return;
			}
			it->second->texture = _texture;
		}
		void Material::setFont(std::shared_ptr<Font> _font, const std::string_view _uniformName, const uint8_t _slot)
		{
			if (_uniformName.empty())
			{
				log::error("Empty material font uniform name!");
				return;
			}
			std::unique_ptr<MaterialFont>& materialFont = fonts[_slot];
			if (!materialFont)
			{
				materialFont = std::make_unique<MaterialFont>();
				materialFont->uniform = std::make_unique<Uniform>(_uniformName, UniformType::Sampler);
			}
			materialFont->font = _font;
			fontChangedSignal();
		}
		void Material::setFont(std::shared_ptr<Font> _font, const uint8_t _slot)
		{
			auto it = fonts.find(_slot);
			if (it == fonts.end())
			{
				log::error("Font in slot " + std::to_string(_slot) + " doesn't exist!");
				return;
			}
			it->second->font = _font;
		}
		void Material::setLit(const bool _value)
		{
			lightsEnabled = _value;
		}
	}
}
