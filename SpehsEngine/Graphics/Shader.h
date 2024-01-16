#pragma once

#include "SpehsEngine/Core/Asset.h"


namespace se::gfx
{
	class Shader : public IAsset
	{
	public:

		~Shader() = default;

		// Construct empty shader
		Shader(std::string_view _name);

		// Construct and load shader from file
		Shader(std::string_view _name, AsyncTaskManager* _taskManager,
			std::string_view _vertexShaderPath, std::string_view _fragmentShaderPath);

		// Construct and load embedded shader
		Shader(std::string_view _name, AsyncTaskManager* _taskManager,
			AssetHandle _vertexShaderHandle, AssetHandle _fragmentShaderHandle);

		bool isReloadable() const override;
		void reload(AsyncTaskManager* _taskManager) override;

		// Load shader from file
		void load(AsyncTaskManager* _taskManager,
			std::string_view _vertexShaderPath, std::string_view _fragmentShaderPath);

		// Load embedded shader
		void load(AsyncTaskManager* _taskManager,
			AssetHandle _vertexShaderHandle, AssetHandle _fragmentShaderHandle);

		const std::string& getVertexShaderPath() const;
		const std::string& getFragmentShaderPath() const;

	private:

		std::string vertexShaderPath;
		std::string fragmentShaderPath;
	};
}
