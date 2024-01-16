#include "stdafx.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"

#include "SpehsEngine/Graphics/Shader.h"
#include "SpehsEngine/Graphics/Uniform.h"
#include "SpehsEngine/Graphics/DefaultShaders.h"


PhongAttributes::PhongAttributes()
{
	attributesUniform = std::make_unique<Uniform>("u_phong_ShininessStrength", UniformType::Vec4);
}
void PhongAttributes::bind()
{
	attributesUniform->set(&data);
}
float& PhongAttributes::shininess()
{
	return data.x;
}
float& PhongAttributes::specularStrength()
{
	return data.y;
}


namespace se::gfx
{
	std::shared_ptr<Material> createMaterial(const DefaultMaterialType _type, const AssetManager& _assetManager)
	{
		switch (_type)
		{
			case DefaultMaterialType::FlatColor:
			{
				std::shared_ptr<Material> material = std::make_shared<Material>();
				material->setShader(_assetManager.find<Shader>(getDefaultShaderName(DefaultShaderType::Color)), ShaderVariant::Default);
				return material;
			}
			case DefaultMaterialType::FlatTexture:
			{
				std::shared_ptr<Material> material = std::make_shared<Material>();
				material->setShader(_assetManager.find<Shader>(getDefaultShaderName(DefaultShaderType::Texture)), ShaderVariant::Default);
				material->setShader(_assetManager.find<Shader>(getDefaultShaderName(DefaultShaderType::Texture_Billboard)), ShaderVariant::Billboard);
				material->setShader(_assetManager.find<Shader>(getDefaultShaderName(DefaultShaderType::Texture_Billboard_Instanced)), ShaderVariant::BillboardInstanced);
				material->setTexture(nullptr, "s_texColor", 0);
				return material;
			}
			case DefaultMaterialType::Skybox:
			{
				std::shared_ptr<Material> material = std::make_shared<Material>();
				material->setShader(_assetManager.find<Shader>(getDefaultShaderName(DefaultShaderType::Skybox)), ShaderVariant::Default);
				material->setTexture(nullptr, "s_texColor", 0);
				return material;
			}
			case DefaultMaterialType::Text:
			{
				std::shared_ptr<Material> material = std::make_shared<Material>();
				material->setShader(_assetManager.find<Shader>(getDefaultShaderName(DefaultShaderType::Text)), ShaderVariant::Default);
				material->setFont(nullptr, "s_fontTex", 0);
				return material;
			}
			case DefaultMaterialType::Phong:
			{
				std::shared_ptr<Material> material = std::make_shared<Material>();
				material->setShader(_assetManager.find<Shader>(getDefaultShaderName(DefaultShaderType::Phong)), ShaderVariant::Default);
				material->setShader(_assetManager.find<Shader>(getDefaultShaderName(DefaultShaderType::Phong_Instanced)), ShaderVariant::Instanced);
				material->setShader(_assetManager.find<Shader>(getDefaultShaderName(DefaultShaderType::Phong_Skinned)), ShaderVariant::Skinned);
				material->setShader(_assetManager.find<Shader>(getDefaultShaderName(DefaultShaderType::Phong_Skinned_Instanced)), ShaderVariant::SkinnedInstanced);
				material->setTexture(nullptr, "s_texColor", PhongTextureType::Color);
				material->setTexture(nullptr, "s_texNormal", PhongTextureType::Normal);
				material->setLit(true);
				material->setUniformContainer(std::make_shared<PhongAttributes>(), "PhongAttributes");
				return material;
			}
			case DefaultMaterialType::PhongCubemap:
			{
				std::shared_ptr<Material> material = std::make_shared<Material>();
				material->setShader(_assetManager.find<Shader>(getDefaultShaderName(DefaultShaderType::Phong_Cubemap)), ShaderVariant::Default);
				material->setTexture(nullptr, "s_texColor", PhongTextureType::Color);
				material->setTexture(nullptr, "s_texNormal", PhongTextureType::Normal);
				material->setTexture(nullptr, "s_texRoughness", PhongTextureType::Roughness);
				material->setLit(true);
				material->setUniformContainer(std::make_shared<PhongAttributes>(), "PhongAttributes");
				return material;
			}
		}

		log::error("Invalid DefaultMaterialType! (" + std::to_string((int)_type) + ")");
		return nullptr;
	}
}
