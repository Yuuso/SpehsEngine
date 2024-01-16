#include "stdafx.h"
#include "SpehsEngine/Graphics/Impl/ShaderLoad.h"

#include "SpehsEngine/Core/File/File.h"
#include "SpehsEngine/Graphics/Impl/AssetData.h"


namespace se::gfx::impl
{
	std::shared_ptr<AssetData> loadShader(AssetHandle _vsHandle, AssetHandle _fsHandle)
	{
		bgfx::ShaderHandle vertexShaderHandle = { _vsHandle.u16handle };
		bgfx::ShaderHandle fragmentShaderHandle = { _fsHandle.u16handle };
		bgfx::ProgramHandle programHandle = bgfx::createProgram(vertexShaderHandle, fragmentShaderHandle, false);
		if (!bgfx::isValid(programHandle))
		{
			log::error("Failed to create shader program!");
			bgfx::destroy(vertexShaderHandle);
			bgfx::destroy(fragmentShaderHandle);
			return nullptr;
		}

		std::shared_ptr<ShaderData> shaderData = std::make_shared<ShaderData>();
		shaderData->init(programHandle, vertexShaderHandle, fragmentShaderHandle);
		return shaderData;
	}
	std::shared_ptr<AssetData> loadShader(std::string _vsPath, std::string _fsPath)
	{
		File vertexShaderFile;
		if (!readFile(vertexShaderFile, _vsPath))
		{
			log::error("Cannot create shader, file read failed! (" + _vsPath + ")");
			return nullptr;
		}

		File fragmentShaderFile;
		if (!readFile(fragmentShaderFile, _fsPath))
		{
			log::error("Cannot create shader, file read failed! (" + _fsPath + ")");
			return nullptr;
		}

		const bgfx::Memory* vertexBuffer = bgfx::copy(vertexShaderFile.data.data(), uint32_t(vertexShaderFile.data.size()));
		const bgfx::Memory* fragmentBuffer = bgfx::copy(fragmentShaderFile.data.data(), uint32_t(fragmentShaderFile.data.size()));
		bgfx::ShaderHandle vertexShader = bgfx::createShader(vertexBuffer);
		bgfx::ShaderHandle fragmentShader = bgfx::createShader(fragmentBuffer);
		 return loadShader(AssetHandle{ vertexShader.idx }, AssetHandle{ fragmentShader.idx });
	}
}
