#include "stdafx.h"
#include "SpehsEngine/Graphics/Material.h"


namespace se
{
	namespace graphics
	{
		void Material::bind()
		{
			uint8_t stage = 0;
			if (font)
				fontUniform->set(*font.get(), stage++);
			for (auto&& texture : textures)
				texture.second->uniform->set(*texture.second->texture.get(), stage++);
			internalBind(stage);
		}

		std::shared_ptr<Shader> Material::getShader() const
		{
			return shader;
		}
		std::shared_ptr<Font> Material::getFont() const
		{
			return font;
		}
		std::shared_ptr<Texture> Material::getTexture(const std::string& _name) const
		{
			auto it = textures.find(_name);
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
		void Material::setTexture(std::shared_ptr<Texture> _texture, const std::string& _name)
		{
			if (_name.empty())
			{
				log::error("Empty material texture name!");
				return;
			}
			std::unique_ptr<MaterialTexture>& materialTexture = textures[_name];
			if (!materialTexture)
			{
				materialTexture = std::make_unique<MaterialTexture>();
				materialTexture->uniform = std::make_unique<Uniform>(_name, UniformType::Sampler);
			}
			materialTexture->texture = _texture;
		}
		void Material::setFont(std::shared_ptr<Font> _font)
		{
			if (!fontUniform)
				fontUniform = std::make_unique<Uniform>("s_fontTex", UniformType::Sampler);
			font = _font;
			fontChangedSignal();
		}
	}
}
