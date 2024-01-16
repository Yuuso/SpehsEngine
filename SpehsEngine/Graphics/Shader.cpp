#include "stdafx.h"
#include "SpehsEngine/Graphics/Shader.h"

#include "SpehsEngine/Graphics/Impl/ShaderLoad.h"



Shader::Shader(std::string_view _name)
	: IAsset(_name)
{}
Shader::Shader(std::string_view _name, AsyncTaskManager* _taskManager,
	   std::string_view _vertexShaderPath, std::string_view _fragmentShaderPath)
	: Shader(_name)
{
	load(_taskManager, _vertexShaderPath, _fragmentShaderPath);
}
Shader::Shader(std::string_view _name, AsyncTaskManager* _taskManager,
	   AssetHandle _vertexShaderHandle, AssetHandle _fragmentShaderHandle)
	: Shader(_name)
{
	load(_taskManager, _vertexShaderHandle, _fragmentShaderHandle);
}
bool Shader::isReloadable() const
{
	return !vertexShaderPath.empty() && !fragmentShaderPath.empty();
}
void Shader::reload(AsyncTaskManager* _taskManager)
{
	if (!isReloadable())
	{
		log::warning("Shader " + getName() + " not reloadable!");
		return;
	}
	load(_taskManager, vertexShaderPath, fragmentShaderPath);
}
void Shader::load(AsyncTaskManager* _taskManager,
	std::string_view _vertexShaderPath, std::string_view _fragmentShaderPath)
{
	vertexShaderPath = _vertexShaderPath;
	fragmentShaderPath = _fragmentShaderPath;
	IAsset::load(_taskManager, [this]{ return loadShader(vertexShaderPath, fragmentShaderPath); });
}
void Shader::load(AsyncTaskManager* _taskManager,
	AssetHandle _vertexShaderHandle, AssetHandle _fragmentShaderHandle)
{
	vertexShaderPath.clear();
	fragmentShaderPath.clear();
	IAsset::load(_taskManager,
		[this, _vertexShaderHandle, _fragmentShaderHandle]
		{
			return loadShader(_vertexShaderHandle, _fragmentShaderHandle);
		});
}
const std::string& Shader::getVertexShaderPath() const
{
	return vertexShaderPath;
}
const std::string& Shader::getFragmentShaderPath() const
{
	return fragmentShaderPath;
}





