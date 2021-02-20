#include "stdafx.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"


namespace se
{
	namespace graphics
	{
		std::shared_ptr<Uniform> makePhongAttributesUniform()
		{
			return std::make_shared<Uniform>("u_phong_ShininessStrength", UniformType::Vec4);
		}

		void bind(std::shared_ptr<MaterialTexture>& _texture, const uint8_t _stage)
		{
			if (_texture->texture)
				_texture->uniform->set(_texture->texture, _stage);
		}
		void bind(std::vector<std::shared_ptr<MaterialTexture>>& _texture)
		{
			uint8_t stage = 0;
			for (auto&& texture : _texture)
			{
				bind(texture, stage++);
			}
		}
		void bind(const PhongAttributes& _attributes, std::shared_ptr<Uniform> uniform)
		{
			static_assert(sizeof(PhongAttributes) == sizeof(glm::vec4), "Invalid PhongAttributes size!");
			uniform->set(&_attributes.data);
		}
		void bind(std::shared_ptr<MaterialFont>& _font, const uint8_t _stage)
		{
			if (_font->font)
				_font->uniform->set(_font->font, _stage);
		}
		void bind(std::vector<std::shared_ptr<MaterialFont>>& _fonts)
		{
			uint8_t stage = 0;
			for (auto&& font : _fonts)
			{
				bind(font, stage++);
			}
		}



		FlatColorMaterial::FlatColorMaterial(DefaultShaderManager& _shaderManager)
		{
			shader = _shaderManager.find("color");
		}

		void FlatColorMaterial::bind()
		{
		}
		std::shared_ptr<Shader> FlatColorMaterial::getShader()
		{
			return shader;
		}



		FlatTextureMaterial::FlatTextureMaterial(DefaultShaderManager& _shaderManager)
		{
			shader = _shaderManager.find("tex");

			texture = std::make_shared<MaterialTexture>();
			texture->type = MaterialTextureType::Color;
			texture->uniform = std::make_shared<Uniform>("s_texColor", UniformType::Sampler);
		}

		void FlatTextureMaterial::bind()
		{
			graphics::bind(texture);
		}
		std::shared_ptr<Shader> FlatTextureMaterial::getShader()
		{
			return shader;
		}
		std::shared_ptr<Texture> FlatTextureMaterial::getTexture(const size_t _index)
		{
			if (_index != 0)
			{
				log::error("FlatTextureMaterial: Invalid texture index!");
				return nullptr;
			}
			return texture->texture;
		}

		void FlatTextureMaterial::setTexture(std::shared_ptr<Texture> _texture)
		{
			texture->texture = _texture;
		}



		PhongMaterial::PhongMaterial(DefaultShaderManager& _shaderManager)
		{
			shader = _shaderManager.find("phong");

			textures.resize(2);
			textures[0] = std::make_shared<MaterialTexture>();
			textures[0]->type = MaterialTextureType::Color;
			textures[0]->uniform = std::make_shared<Uniform>("s_texColor", UniformType::Sampler);
			textures[1] = std::make_shared<MaterialTexture>();
			textures[1]->type = MaterialTextureType::Normal;
			textures[1]->uniform = std::make_shared<Uniform>("s_texNormal", UniformType::Sampler);

			attributesUniform = makePhongAttributesUniform();
		}

		void PhongMaterial::bind()
		{
			graphics::bind(textures);
			graphics::bind(attributes, attributesUniform);
		}
		std::shared_ptr<Shader> PhongMaterial::getShader()
		{
			return shader;
		}
		std::shared_ptr<Texture> PhongMaterial::getTexture(const size_t _index)
		{
			if (_index == 0)
			{
				return getTexture(MaterialTextureType::Color);
			}
			else if (_index == 1)
			{
				return getTexture(MaterialTextureType::Normal);
			}
			else
			{
				log::error("PhongMaterial: Invalid texture index!");
				return nullptr;
			}
		}

		void PhongMaterial::setTexture(const MaterialTextureType _type, std::shared_ptr<Texture> _texture)
		{
			auto it = std::find_if(textures.begin(), textures.end(), [_type](const std::shared_ptr<MaterialTexture>& _tex) { return _tex->type == _type; });
			if (it != textures.end())
			{
				it->get()->texture = _texture;
				return;
			}
			log::error("Texture of type '" + std::to_string((int)_type) + "' not found in PhongMaterial!");
		}
		std::shared_ptr<Texture> PhongMaterial::getTexture(const MaterialTextureType _type)
		{
			auto it = std::find_if(textures.begin(), textures.end(), [_type](const std::shared_ptr<MaterialTexture>& _tex) { return _tex->type == _type; });
			if (it != textures.end())
				return it->get()->texture;
			log::error("Texture of type '" + std::to_string((int)_type) + "' not found in PhongMaterial!");
			return nullptr;
		}

		void PhongMaterial::setAttributes(const PhongAttributes& _attributes)
		{
			attributes = _attributes;
		}
		PhongAttributes& PhongMaterial::getAttributes()
		{
			return attributes;
		}



		TextMaterial::TextMaterial(DefaultShaderManager& _shaderManager)
		{
			shader = _shaderManager.find("text");

			font = std::make_shared<MaterialFont>();
			font->uniform = std::make_shared<Uniform>("s_fontTex", UniformType::Sampler);
		}

		void TextMaterial::bind()
		{
			graphics::bind(font);
		}
		std::shared_ptr<Shader> TextMaterial::getShader()
		{
			return shader;
		}
		std::shared_ptr<Font> TextMaterial::getFont(const size_t _index)
		{
			if (_index != 0)
			{
				log::error("TextMaterial: Invalid font index!");
			}
			return font->font;
		}

		void TextMaterial::setFont(std::shared_ptr<Font> _font, const size_t _index)
		{
			if (_index != 0)
			{
				log::error("TextMaterial: Invalid font index!");
			}
			font->font = _font;
		}
	}
}
