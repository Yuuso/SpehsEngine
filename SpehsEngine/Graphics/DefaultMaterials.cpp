#include "stdafx.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"


namespace se
{
	namespace graphics
	{
		void Bind(std::unique_ptr<MaterialTexture>& _tex, const uint8_t _stage)
		{
			if (_tex->texture)
				_tex->uniform->set(_tex->texture, _stage);
		}
		void Bind(std::vector<std::unique_ptr<MaterialTexture>>& _tex)
		{
			uint8_t stage = 0;
			for (size_t i = 0; i < _tex.size(); i++)
			{
				Bind(_tex[i], stage++);
			}
		}
		void Bind(const PhongAttributes& _tex, std::shared_ptr<Uniform> uniform)
		{
			static_assert(sizeof(PhongAttributes) == sizeof(glm::vec4), "Invalid PhongAttributes size!");
			uniform->set(reinterpret_cast<const float*>(&_tex), 4);
		}



		FlatColorMaterial::FlatColorMaterial(DefaultShaderManager& _shaderManager)
		{
			shader = _shaderManager.findShader("color");
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
			shader = _shaderManager.findShader("tex");

			texture = std::make_unique<MaterialTexture>();
			texture->type = MaterialTextureType::Color;
			texture->uniform = _shaderManager.findUniform("s_texColor");
		}

		void FlatTextureMaterial::bind()
		{
			Bind(texture);
		}
		std::shared_ptr<Shader> FlatTextureMaterial::getShader()
		{
			return shader;
		}

		void FlatTextureMaterial::setTexture(std::shared_ptr<Texture> _texture)
		{
			texture->texture = _texture;
		}
		std::shared_ptr<Texture> FlatTextureMaterial::getTexture()
		{
			return texture->texture;
		}



		PhongMaterial::PhongMaterial(DefaultShaderManager& _shaderManager)
		{
			shader = _shaderManager.findShader("phong");

			textures.resize(2);
			textures[0] = std::make_unique<MaterialTexture>();
			textures[0]->type = MaterialTextureType::Color;
			textures[0]->uniform = _shaderManager.findUniform("s_texColor");
			textures[1] = std::make_unique<MaterialTexture>();
			textures[1]->type = MaterialTextureType::Normal;
			textures[1]->uniform = _shaderManager.findUniform("s_texNormal");

			attributesUniform = _shaderManager.findUniform(PhongAttributesUniformName);
		}

		void PhongMaterial::bind()
		{
			Bind(textures);
			Bind(attributes, attributesUniform);
		}
		std::shared_ptr<Shader> PhongMaterial::getShader()
		{
			return shader;
		}

		void PhongMaterial::setTexture(const MaterialTextureType _type, std::shared_ptr<Texture> _texture)
		{
			auto it = std::find_if(textures.begin(), textures.end(), [_type](const std::unique_ptr<MaterialTexture>& _tex) { return _tex->type == _type; });
			if (it != textures.end())
			{
				it->get()->texture = _texture;
				return;
			}
			log::error("Texture of type '" + std::to_string((int)_type) + "' not found in PhongMaterial!");
		}
		std::shared_ptr<Texture> PhongMaterial::getTexture(const MaterialTextureType _type)
		{
			auto it = std::find_if(textures.begin(), textures.end(), [_type](const std::unique_ptr<MaterialTexture>& _tex) { return _tex->type == _type; });
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
	}
}
