#include "stdafx.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"


namespace se
{
	namespace graphics
	{
		FlatColorMaterial::FlatColorMaterial(DefaultShaderManager& _shaderManager)
		{
			setShader(_shaderManager.find("color"));
		}
		void FlatColorMaterial::internalBind(uint8_t /*stage*/)
		{
		}


		static const std::string flatTextureName = "s_texColor";

		FlatTextureMaterial::FlatTextureMaterial(DefaultShaderManager& _shaderManager)
		{
			setShader(_shaderManager.find("tex"));
		}
		void FlatTextureMaterial::setTexture(std::shared_ptr<Texture> _texture)
		{
			Material::setTexture(_texture, flatTextureName);
		}
		std::shared_ptr<Texture> FlatTextureMaterial::getTexture() const
		{
			return Material::getTexture(flatTextureName);
		}
		void FlatTextureMaterial::internalBind(uint8_t /*stage*/)
		{
		}



		TextMaterial::TextMaterial(DefaultShaderManager& _shaderManager)
		{
			setShader(_shaderManager.find("text"));
		}
		void TextMaterial::internalBind(uint8_t /*stage*/)
		{
		}


		static const std::string phongTextureName(const PhongTextureType _type)
		{
			switch (_type)
			{
				case PhongTextureType::Color:
					return "s_texColor";
				case PhongTextureType::Normal:
					return "s_texNormal";
			}
			se_assert(false);
			return "";
		}

		PhongMaterial::PhongMaterial(DefaultShaderManager& _shaderManager)
		{
			setShader(_shaderManager.find("phong"));
			attributesUniform = std::make_unique<Uniform>("u_phong_ShininessStrength", UniformType::Vec4);
		}
		void PhongMaterial::setTexture(const PhongTextureType _type, std::shared_ptr<Texture> _texture)
		{
			Material::setTexture(_texture, phongTextureName(_type));
		}
		std::shared_ptr<Texture> PhongMaterial::getTexture(const PhongTextureType _type) const
		{
			return Material::getTexture(phongTextureName(_type));
		}
		void PhongMaterial::setAttributes(const PhongAttributes& _attributes)
		{
			attributes = _attributes;
		}
		PhongAttributes& PhongMaterial::getAttributes()
		{
			return attributes;
		}
		void PhongMaterial::internalBind(uint8_t /*stage*/)
		{
			attributesUniform->set(&attributes.data);
		}
	}
}
