#include "stdafx.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"

#include "SpehsEngine/Graphics/ShaderManager.h"
#include "SpehsEngine/Graphics/Uniform.h"


namespace se
{
	namespace graphics
	{
		PhongAttributes::PhongAttributes()
		{
			attributesUniform = std::make_unique<Uniform>("u_phong_ShininessStrength", UniformType::Vec4);
		}
		PhongAttributes::~PhongAttributes()
		{
			// ~Uniform()
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


		std::shared_ptr<Material> createMaterial(const DefaultMaterialType _type, ShaderManager& _shaderManager)
		{
			switch (_type)
			{
				case se::graphics::DefaultMaterialType::FlatColor:
				{
					std::shared_ptr<Material> material = std::make_shared<Material>();
					material->setShader(_shaderManager.find("color"), ShaderVariant::Default);
					return material;
				}
				case se::graphics::DefaultMaterialType::FlatTexture:
				{
					std::shared_ptr<Material> material = std::make_shared<Material>();
					material->setShader(_shaderManager.find("tex"), ShaderVariant::Default);
					material->setShader(_shaderManager.find("tex_billboard"), ShaderVariant::Billboard);
					material->setShader(_shaderManager.find("tex_billboard_instanced"), ShaderVariant::BillboardInstanced);
					material->setTexture(nullptr, "s_texColor", 0);
					return material;
				}
				case se::graphics::DefaultMaterialType::Skybox:
				{
					std::shared_ptr<Material> material = std::make_shared<Material>();
					material->setShader(_shaderManager.find("skybox"), ShaderVariant::Default);
					material->setTexture(nullptr, "s_texColor", 0);
					return material;
				}
				case se::graphics::DefaultMaterialType::Text:
				{
					std::shared_ptr<Material> material = std::make_shared<Material>();
					material->setShader(_shaderManager.find("text"), ShaderVariant::Default);
					material->setFont(nullptr, "s_fontTex", 0);
					return material;
				}
				case se::graphics::DefaultMaterialType::Phong:
				{
					std::shared_ptr<Material> material = std::make_shared<Material>();
					material->setShader(_shaderManager.find("phong"), ShaderVariant::Default);
					material->setShader(_shaderManager.find("phong_instanced"), ShaderVariant::Instanced);
					material->setShader(_shaderManager.find("phong_skinned"), ShaderVariant::Skinned);
					material->setShader(_shaderManager.find("phong_skinned_instanced"), ShaderVariant::SkinnedInstanced);
					material->setTexture(nullptr, "s_texColor", PhongTextureType::Color);
					material->setTexture(nullptr, "s_texNormal", PhongTextureType::Normal);
					material->setLit(true);
					material->setUniformContainer(std::make_shared<PhongAttributes>(), "PhongAttributes");
					return material;
				}
				case se::graphics::DefaultMaterialType::PhongCubemap:
				{
					std::shared_ptr<Material> material = std::make_shared<Material>();
					material->setShader(_shaderManager.find("phong_cubemap"), ShaderVariant::Default);
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
}
