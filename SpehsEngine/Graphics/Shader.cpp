#include "stdafx.h"
#include "SpehsEngine/Graphics/Shader.h"

#include "SpehsEngine/Core/File/File.h"


namespace se
{
	namespace graphics
	{
		Shader::Shader(const std::string_view _name)
			: Resource(_name)
		{
		}
		Shader::~Shader()
		{
			destroy();
		}

		void Shader::destroy()
		{
			if (!resourceData)
				return;

			safeDestroy<bgfx::ProgramHandle>(resourceData->handle);
			safeDestroy<bgfx::ShaderHandle>(resourceData->vertexShaderHandle);
			safeDestroy<bgfx::ShaderHandle>(resourceData->fragmentShaderHandle);

			resourceData.reset();
		}

		void Shader::reload(std::shared_ptr<ResourceLoader> _resourceLoader)
		{
			if (!reloadable())
			{
				log::error("Cannot reload shader!");
				return;
			}
			destroy();
			create(vertexShaderPath, fragmentShaderPath, _resourceLoader);
		}

		bool Shader::reloadable() const
		{
			return !vertexShaderPath.empty() && !fragmentShaderPath.empty();
		}

		std::shared_ptr<ResourceData> Shader::createResource(const std::string _vertexShaderPath, const std::string _fragmentShaderPath)
		{
			File vertexShaderFile;
			File fragmentShaderFile;
			if (!readFile(vertexShaderFile, _vertexShaderPath))
			{
				log::error("Cannot create shader, file read failed! (" + _vertexShaderPath + ")");
				return nullptr;
			}
			if (!readFile(fragmentShaderFile, _fragmentShaderPath))
			{
				log::error("Cannot create shader, file read failed! (" + _fragmentShaderPath + ")");
				return nullptr;
			}
			const bgfx::Memory* vertexBuffer = bgfx::copy(vertexShaderFile.data.data(), uint32_t(vertexShaderFile.data.size()));
			const bgfx::Memory* fragmentBuffer = bgfx::copy(fragmentShaderFile.data.data(), uint32_t(fragmentShaderFile.data.size()));
			bgfx::ShaderHandle vertexShader = bgfx::createShader(vertexBuffer);
			bgfx::ShaderHandle fragmentShader = bgfx::createShader(fragmentBuffer);
			return createResourceFromHandles(vertexShader.idx, fragmentShader.idx);
		}
		std::shared_ptr<ResourceData> Shader::createResourceFromHandles(ResourceHandle _vertexShaderHandle, ResourceHandle _fragmentShaderHandle)
		{
			bgfx::ShaderHandle vertexShaderHandle = { _vertexShaderHandle };
			bgfx::ShaderHandle fragmentShaderHandle = { _fragmentShaderHandle };
			bgfx::ProgramHandle programHandle = bgfx::createProgram(vertexShaderHandle, fragmentShaderHandle, false);
			if (!bgfx::isValid(programHandle))
			{
				log::error("Failed to create shader program!");

				bgfx::destroy(vertexShaderHandle);
				bgfx::destroy(fragmentShaderHandle);

				return nullptr;
			}

			std::shared_ptr<ShaderData> shaderData = std::make_shared<ShaderData>();
			shaderData->vertexShaderHandle = _vertexShaderHandle;
			shaderData->fragmentShaderHandle = _fragmentShaderHandle;
			shaderData->handle = programHandle.idx;
			return shaderData;
		}

		void Shader::create(const std::string_view _vertexShaderPath, const std::string_view _fragmentShaderPath, std::shared_ptr<ResourceLoader> _resourceLoader)
		{
			se_assert(!resourceData);

			vertexShaderPath = _vertexShaderPath;
			fragmentShaderPath = _fragmentShaderPath;

			if (_resourceLoader)
			{
				std::function<std::shared_ptr<ResourceData>()> func = std::bind(&Shader::createResource, vertexShaderPath, fragmentShaderPath);
				resourceFuture = _resourceLoader->push(func);
			}
			else
			{
				resourceData = std::dynamic_pointer_cast<ShaderData>(createResource(vertexShaderPath, fragmentShaderPath));
			}
		}
		void Shader::create(ResourceHandle _vertexShaderHandle, ResourceHandle _fragmentShaderHandle)
		{
			destroy();
			resourceData = std::dynamic_pointer_cast<ShaderData>(createResourceFromHandles(_vertexShaderHandle, _fragmentShaderHandle));
		}
	}
}
