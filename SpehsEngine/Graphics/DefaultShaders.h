#pragma once

#include "SpehsEngine/Core/AssetManager.h"
#include "SpehsEngine/Graphics/Shader.h"


namespace se::gfx
{
	enum class DefaultShaderType
	{
		Color,
		Texture,
		Texture_Billboard,
		Texture_Billboard_Instanced,
		Phong,
		Phong_Cubemap,
		Phong_Instanced,
		Phong_Skinned,
		Phong_Skinned_Instanced,
		Skybox,
		Text,

		Count
	};

	const char* getDefaultShaderName(DefaultShaderType _type);
	std::shared_ptr<Shader> createDefaultShader(DefaultShaderType _type, AsyncTaskManager* _taskManager);
	void createDefaultShaders(AssetManager& _manager);
}
