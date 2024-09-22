#include "stdafx.h"
#include "SpehsEngine/Graphics/Material.h"

#include "SpehsEngine/Graphics/Font.h"
#include "SpehsEngine/Graphics/Shader.h"
#include "SpehsEngine/Graphics/Texture.h"
#include "SpehsEngine/Graphics/Uniform.h"


namespace se
{
	namespace graphics
	{
		void Material::bind()
		{
			for (auto&& font : fonts)
			{
				se_assert_m(font.second->font, "Material '" + getName() + "' missing font in slot " + std::to_string(font.first) + ".");
				if (font.second->font)
					font.second->uniform->set(*font.second->font.get(), font.first);
			}
			for (auto&& texture : textures)
			{
				se_assert_m(texture.second->texture, "Material '" + getName() + "' missing texture in slot " + std::to_string(texture.first) + ".");
				if (texture.second->texture)
					texture.second->uniform->set(*texture.second->texture.get(), texture.first);
			}
			for (auto&& uniformContainer : uniformContainers)
			{
				se_assert_m(uniformContainer.second, "Material '" + getName() + "' missing uniform container '" + uniformContainer.first + "'.");
				if (uniformContainer.second)
					uniformContainer.second->bind();
			}
		}
		void Material::connectToFontChangedSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(void)>& callback)
		{
			scopedConnection = fontChangedSignal.connect(callback);
		}

		const std::shared_ptr<Shader> Material::getShader(ShaderVariant variant) const
		{
			auto it = shaders.find(variant);
			if (it != shaders.end())
				return it->second;
			log::error("Material '" + name + "' is missing the shader variant '" + toString(variant) + "'!");

			it = shaders.find(ShaderVariant::Default);
			if (it != shaders.end())
				return it->second;
			log::error("Material '" + name + "'  is also missing the default shader variant!");

			return nullptr;
		}
		const std::shared_ptr<Font> Material::getFont(uint8_t _slot) const
		{
			auto it = fonts.find(_slot);
			if (it == fonts.end())
				return nullptr;
			return it->second->font;
		}
		const std::shared_ptr<Texture> Material::getTexture(uint8_t _slot) const
		{
			auto it = textures.find(_slot);
			if (it == textures.end())
				return nullptr;
			return it->second->texture;
		}
		bool Material::checkUniformContainer(const std::string& _name) const
		{
			auto it = uniformContainers.find(_name);
			return it != uniformContainers.end();
		}
		bool Material::getLit() const
		{
			return lightsEnabled;
		}
		const std::string& Material::getName() const
		{
			return name;
		}

		void Material::setShader(std::shared_ptr<Shader> _shader, ShaderVariant variant)
		{
			shaders[variant] = _shader;
		}
		void Material::setTexture(std::shared_ptr<Texture> _texture, std::string_view _uniformName, uint8_t _slot)
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
		void Material::setTexture(std::shared_ptr<Texture> _texture, uint8_t _slot)
		{
			auto it = textures.find(_slot);
			if (it == textures.end())
			{
				log::error("Texture in slot " + std::to_string(_slot) + " doesn't exist!");
				return;
			}
			it->second->texture = _texture;
		}
		void Material::setFont(std::shared_ptr<Font> _font, std::string_view _uniformName, uint8_t _slot)
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
		void Material::setFont(std::shared_ptr<Font> _font, uint8_t _slot)
		{
			auto it = fonts.find(_slot);
			if (it == fonts.end())
			{
				log::error("Font in slot " + std::to_string(_slot) + " doesn't exist!");
				return;
			}
			it->second->font = _font;
		}
		void Material::setUniformContainer(std::shared_ptr<UniformContainer> _uniformContainer, const std::string& _name)
		{
			if (_name.empty())
			{
				log::error("Empty uniform container name!");
				return;
			}
			uniformContainers[_name] = _uniformContainer;
		}
		void Material::setLit(bool _value)
		{
			lightsEnabled = _value;
		}
		void Material::setName(std::string_view _name)
		{
			name = _name;
		}
	}
}
