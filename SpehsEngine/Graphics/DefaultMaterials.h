#pragma once

#include "SpehsEngine/Graphics/DefaultShaderManager.h"
#include "SpehsEngine/Graphics/Material.h"
#include "SpehsEngine/Graphics/Shader.h"
#include "SpehsEngine/Graphics/Texture.h"
#include "SpehsEngine/Graphics/Uniform.h"
#include <memory>
#include <string>
#include <vector>


namespace se
{
	namespace graphics
	{
		enum class MaterialTextureType
		{
			Unknown,
			Color,
			Normal,
		};

		struct MaterialTexture
		{
			MaterialTextureType type = MaterialTextureType::Unknown;
			std::shared_ptr<Texture> texture = nullptr;
			std::shared_ptr<Uniform> uniform = nullptr;
		};
		void Bind(std::unique_ptr<MaterialTexture>& _tex, const uint8_t _stage = 0);
		void Bind(std::vector<std::unique_ptr<MaterialTexture>>& _tex);


		struct PhongAttributes
		{
			float shininess = 0.5f;
			float specularStrength = 32.0f;
			float __pad[2] = { 0.0f, 0.0f };
		};
		constexpr std::string_view PhongAttributesUniformName = "u_phong_ShininessStrength";
		void Bind(const PhongAttributes& _tex, std::shared_ptr<Uniform> uniform);



		class FlatColorMaterial final : public Material
		{
		public:

			FlatColorMaterial(DefaultShaderManager& _shaderManager);

			void bind() override;
			std::shared_ptr<Shader> getShader() override;

		private:

			std::shared_ptr<Shader> shader;
		};


		class FlatTextureMaterial final : public Material
		{
		public:

			FlatTextureMaterial(DefaultShaderManager& _shaderManager);

			void bind() override;
			std::shared_ptr<Shader> getShader() override;

			void setTexture(std::shared_ptr<Texture> _texture);
			std::shared_ptr<Texture> getTexture();

		private:

			std::shared_ptr<Shader> shader;
			std::unique_ptr<MaterialTexture> texture;
		};


		class PhongMaterial final : public Material
		{
		public:

			PhongMaterial(DefaultShaderManager& _shaderManager);

			void bind() override;
			std::shared_ptr<Shader> getShader() override;

			void setTexture(const MaterialTextureType _type, std::shared_ptr<Texture> _texture);
			std::shared_ptr<Texture> getTexture(const MaterialTextureType _type);

			void setAttributes(const PhongAttributes& _attributes);
			PhongAttributes& getAttributes();

		private:

			std::shared_ptr<Shader> shader;
			std::vector<std::unique_ptr<MaterialTexture>> textures;
			PhongAttributes attributes;
			std::shared_ptr<Uniform> attributesUniform;
		};
	}
}
