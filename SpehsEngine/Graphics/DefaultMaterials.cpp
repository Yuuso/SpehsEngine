#include "stdafx.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"


namespace se
{
	namespace graphics
	{
		const std::string_view phongTextureUniformName(const PhongTextureType _type)
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



		FlatColorMaterial::FlatColorMaterial(ShaderManager& _shaderManager)
		{
			setShader(_shaderManager.find("color"));
		}
		void FlatColorMaterial::internalBind()
		{
		}



		FlatTextureMaterial::FlatTextureMaterial(ShaderManager& _shaderManager)
		{
			setShader(_shaderManager.find("tex"), ShaderVariant::Default);
			setShader(_shaderManager.find("tex_billboard"), ShaderVariant::Billboard);
			setShader(_shaderManager.find("tex_billboard_instanced"), ShaderVariant::BillboardInstanced);
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



		SkyboxMaterial::SkyboxMaterial(ShaderManager& _shaderManager)
		{
			setShader(_shaderManager.find("skybox"), ShaderVariant::Default);
		}
		void SkyboxMaterial::setTexture(std::shared_ptr<Texture> _texture)
		{
			Material::setTexture(_texture, "s_texColor", 0);
		}
		std::shared_ptr<Texture> SkyboxMaterial::getTexture() const
		{
			return Material::getTexture(0);
		}
		void SkyboxMaterial::internalBind()
		{
		}



		TextMaterial::TextMaterial(ShaderManager& _shaderManager)
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


		PhongMaterial::PhongMaterial(ShaderManager& _shaderManager)
		{
			setShader(_shaderManager.find("phong"), ShaderVariant::Default);
			setShader(_shaderManager.find("phong_instanced"), ShaderVariant::Instanced);
			setShader(_shaderManager.find("phong_skinned"), ShaderVariant::Skinned);
			setShader(_shaderManager.find("phong_skinned_instanced"), ShaderVariant::SkinnedInstanced);
			setLit(true);
			attributesUniform = std::make_unique<Uniform>("u_phong_ShininessStrength", UniformType::Vec4);
		}
		void PhongMaterial::setTexture(const PhongTextureType _type, std::shared_ptr<Texture> _texture)
		{
			Material::setTexture(_texture, phongTextureUniformName(_type), static_cast<uint8_t>(_type));
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
	}
}
