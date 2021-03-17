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
		void FlatColorMaterial::internalBind()
		{
		}



		FlatTextureMaterial::FlatTextureMaterial(DefaultShaderManager& _shaderManager)
		{
			setShader(_shaderManager.find("tex"));
		}
		void FlatTextureMaterial::setTexture(std::shared_ptr<Texture> _texture)
		{
			Material::setTexture(_texture, "s_texColor", 0);
		}
		std::shared_ptr<Texture> FlatTextureMaterial::getTexture() const
		{
			return Material::getTexture(0);
		}
		void FlatTextureMaterial::internalBind()
		{
		}



		TextMaterial::TextMaterial(DefaultShaderManager& _shaderManager)
		{
			setShader(_shaderManager.find("text"));
		}
		void TextMaterial::setFont(std::shared_ptr<Font> _font)
		{
			Material::setFont(_font, "s_fontTex", 0);
		}
		std::shared_ptr<Font> TextMaterial::getFont() const
		{
			return Material::getFont(0);
		}
		void TextMaterial::internalBind()
		{
		}


		static const std::string_view phongTextureName(const PhongTextureType _type)
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
			setLit(true);
			attributesUniform = std::make_unique<Uniform>("u_phong_ShininessStrength", UniformType::Vec4);
		}
		void PhongMaterial::setTexture(const PhongTextureType _type, std::shared_ptr<Texture> _texture)
		{
			Material::setTexture(_texture, phongTextureName(_type), static_cast<uint8_t>(_type));
		}
		std::shared_ptr<Texture> PhongMaterial::getTexture(const PhongTextureType _type) const
		{
			return Material::getTexture(static_cast<uint8_t>(_type));
		}
		void PhongMaterial::setAttributes(const PhongAttributes& _attributes)
		{
			attributes = _attributes;
		}
		PhongAttributes& PhongMaterial::getAttributes()
		{
			return attributes;
		}
		void PhongMaterial::internalBind()
		{
			attributesUniform->set(&attributes.data);
		}


		SkinnedPhongMaterial::SkinnedPhongMaterial(DefaultShaderManager& _shaderManager)
			: PhongMaterial(_shaderManager)
		{
			setShader(_shaderManager.find("phong_skinned"));
		}
	}
}
