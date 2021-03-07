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
		enum class PhongTextureType
		{
			Color,
			Normal
		};



		/*
		* Default materials
		*/

		class FlatColorMaterial : public Material
		{
		public:

			FlatColorMaterial(DefaultShaderManager& _shaderManager);

		protected:

			void internalBind(uint8_t stage) override;
		};


		class FlatTextureMaterial : public Material
		{
		public:

			FlatTextureMaterial(DefaultShaderManager& _shaderManager);

			void setTexture(std::shared_ptr<Texture> _texture);
			std::shared_ptr<Texture> getTexture() const;

		protected:

			void internalBind(uint8_t stage) override;
		};


		class TextMaterial : public Material
		{
		public:

			TextMaterial(DefaultShaderManager& _shaderManager);

		protected:

			virtual void internalBind(uint8_t stage) override;
		};


		class PhongMaterial : public Material
		{
		public:

			PhongMaterial(DefaultShaderManager& _shaderManager);

			void setTexture(const PhongTextureType _type, std::shared_ptr<Texture> _texture);
			std::shared_ptr<Texture> getTexture(const PhongTextureType _type) const;

			void setAttributes(const PhongAttributes& _attributes);
			PhongAttributes& getAttributes();

		protected:

			virtual void internalBind(uint8_t stage) override;

			std::unique_ptr<Uniform> attributesUniform;
			PhongAttributes attributes;
		};
	}
}
