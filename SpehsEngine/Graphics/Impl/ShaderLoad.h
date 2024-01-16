#pragma once


namespace se::gfx::impl
{
	std::shared_ptr<AssetData> loadShader(AssetHandle _vertexShaderHandle, AssetHandle _fragmentShaderHandle);
	std::shared_ptr<AssetData> loadShader(std::string _vertexShaderPath, std::string _fragmentShaderPath);
}
