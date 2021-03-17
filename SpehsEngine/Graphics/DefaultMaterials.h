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
			glm::vec4 data = { 32.0f, 0.6f, 0.0f, 0.0f };
		};
		enum class PhongTextureType : uint8_t
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
			virtual ~FlatColorMaterial() = default;

		protected:

			void internalBind() override;
		};


		class FlatTextureMaterial : public Material
		{
		public:

			FlatTextureMaterial(DefaultShaderManager& _shaderManager);
			virtual ~FlatTextureMaterial() = default;

			void setTexture(std::shared_ptr<Texture> _texture);
			std::shared_ptr<Texture> getTexture() const;

		protected:

			void internalBind() override;
		};


		class TextMaterial : public Material
		{
		public:

			TextMaterial(DefaultShaderManager& _shaderManager);
			virtual ~TextMaterial() = default;

			void setFont(std::shared_ptr<Font> _font);
			std::shared_ptr<Font> getFont() const;

		protected:

			virtual void internalBind() override;
		};


		class PhongMaterial : public Material
		{
		public:

			PhongMaterial(DefaultShaderManager& _shaderManager);
			virtual ~PhongMaterial() = default;

			void setTexture(const PhongTextureType _type, std::shared_ptr<Texture> _texture);
			std::shared_ptr<Texture> getTexture(const PhongTextureType _type) const;

			void setAttributes(const PhongAttributes& _attributes);
			PhongAttributes& getAttributes();

		protected:

			virtual void internalBind() override;

			std::unique_ptr<Uniform> attributesUniform;
			PhongAttributes attributes;
		};


		class SkinnedPhongMaterial : public PhongMaterial
		{
		public:
			SkinnedPhongMaterial(DefaultShaderManager& _shaderManager);
			virtual ~SkinnedPhongMaterial() = default;
		};
	}
}
