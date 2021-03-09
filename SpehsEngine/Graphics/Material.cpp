#include "stdafx.h"
#include "SpehsEngine/Graphics/Material.h"


namespace se
{
	namespace graphics
	{
		void Material::bind()
		{
			for (auto&& font : fonts)
				font.second->uniform->set(*font.second->font.get(), font.first);
			for (auto&& texture : textures)
				texture.second->uniform->set(*texture.second->texture.get(), texture.first);
			internalBind();
		}

		std::shared_ptr<Shader> Material::getShader() const
		{
			return shader;
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

		void Material::connectToFontChangedSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<void(void)>& callback)
		{
			scopedConnection = fontChangedSignal.connect(callback);
		}

		void Material::setShader(std::shared_ptr<Shader> _shader)
		{
			shader = _shader;
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

		void Material::setLit(const bool _value)
		{
			lit = _value;
		}
		const bool Material::getLit() const
		{
			return lit;
		}
	}
}
