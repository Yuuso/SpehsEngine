#pragma once

#include "SpehsEngine/Graphics/DefaultShaderManager.h"
#include "SpehsEngine/Graphics/Font.h"
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
		/*
		* Material resources
		*/

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

		struct MaterialFont
		{
			std::shared_ptr<Font> font = nullptr;
			std::shared_ptr<Uniform> uniform = nullptr;
		};

		struct PhongAttributes
		{
			float& shininess()
			{
				return data.x;
			}
			float& specularStrength()
			{
				return data.y;
			}
			glm::vec4 data = { 0.5f, 32.0f, 0.0f, 0.0f };
		};
		std::shared_ptr<Uniform> makePhongAttributesUniform();


		void bind(std::shared_ptr<MaterialTexture>& _texture, const uint8_t _stage = 0);
		void bind(std::vector<std::shared_ptr<MaterialTexture>>& _texture);
		void bind(const PhongAttributes& _attributes, std::shared_ptr<Uniform> _uniform);
		void bind(std::shared_ptr<MaterialFont>& _font, const uint8_t _stage = 0);
		void bind(std::vector<std::shared_ptr<MaterialFont>>& _fonts);



		/*
		* Default materials
		*/

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
			std::shared_ptr<Texture> getTexture(const size_t _index = 0) override;

			void setTexture(std::shared_ptr<Texture> _texture);

		private:

			std::shared_ptr<Shader> shader;
			std::shared_ptr<MaterialTexture> texture;
		};


		class PhongMaterial final : public Material
		{
		public:

			PhongMaterial(DefaultShaderManager& _shaderManager);

			void bind() override;
			std::shared_ptr<Shader> getShader() override;
			std::shared_ptr<Texture> getTexture(const size_t _index = 0) override;

			void setTexture(const MaterialTextureType _type, std::shared_ptr<Texture> _texture);
			std::shared_ptr<Texture> getTexture(const MaterialTextureType _type);

			void setAttributes(const PhongAttributes& _attributes);
			PhongAttributes& getAttributes();

		private:

			std::shared_ptr<Shader> shader;
			std::vector<std::shared_ptr<MaterialTexture>> textures;
			PhongAttributes attributes;
			std::shared_ptr<Uniform> attributesUniform;
		};


		class TextMaterial final : public Material
		{
		public:

			TextMaterial(DefaultShaderManager& _shaderManager);

			void bind() override;
			std::shared_ptr<Shader> getShader() override;
			std::shared_ptr<Font> getFont(const size_t _index = 0) override;

			void setFont(std::shared_ptr<Font> _font, const size_t _index = 0);

		private:

			std::shared_ptr<Shader> shader;
			std::shared_ptr<MaterialFont> font;
		};
	}
}
