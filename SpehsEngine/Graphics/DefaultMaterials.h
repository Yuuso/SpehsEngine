#pragma once

#include "SpehsEngine/Graphics/Material.h"


namespace se::gfx
{
	class ShaderManager;
	class Uniform;

	class PhongAttributes final : public UniformContainer
	{
	public:

		PhongAttributes();
		~PhongAttributes() = default;

		PhongAttributes(const PhongAttributes& _other) = delete;
		PhongAttributes& operator=(const PhongAttributes& _other) = delete;

		PhongAttributes(PhongAttributes&& _other) = delete;
		PhongAttributes& operator=(PhongAttributes&& _other) = delete;


		void bind() override;
		float& shininess();
		float& specularStrength();

	private:

		std::unique_ptr<Uniform> attributesUniform;
		glm::vec4 data{ 32.0f, 0.6f, 0.0f, 0.0f };
	};

	namespace PhongTextureType
	{
		enum : uint8_t
		{
			Color	= 0,
			Normal	= 1,
			Roughness = 2,
		};
	}


	enum class DefaultMaterialType
	{
		FlatColor,
		FlatTexture,
		Skybox,
		Text,
		Phong,
		PhongCubemap,
	};

	std::shared_ptr<Material> createMaterial(const DefaultMaterialType _type, ShaderManager& _shaderManager);
}
