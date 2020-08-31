#include "stdafx.h"
#include "SpehsEngine/Graphics/Shader.h"

#include <bgfx/bgfx.h>
#include "SpehsEngine/Core/File/File.h"
#include "SpehsEngine/Core/Log.h"


namespace se
{
	namespace graphics
	{
		Shader::Shader(const std::string_view _name)
			: name(_name)
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

			bgfx::ProgramHandle programHandle = { resourceData->handle };
			if (bgfx::isValid(programHandle))
			{
				bgfx::destroy(programHandle);
			}

			bgfx::ShaderHandle vertexShaderHandle = { resourceData->vertexShaderHandle };
			if (bgfx::isValid(vertexShaderHandle))
			{
				bgfx::destroy(vertexShaderHandle);
			}

			bgfx::ShaderHandle fragmentShaderHandle = { resourceData->fragmentShaderHandle };
			if (bgfx::isValid(fragmentShaderHandle))
			{
				bgfx::destroy(fragmentShaderHandle);
			}

			resourceData.reset();
		}

		const std::string& Shader::getName() const
		{
			return name;
		}

		void Shader::reload(ResourceLoader _resourceLoader)
		{
			if (vertexShaderPath.empty() || fragmentShaderPath.empty())
			{
				log::error("Cannot reload shader, path not defined!");
				return;
			}
			destroy();
			create(vertexShaderPath, fragmentShaderPath, _resourceLoader);
		}

		std::shared_ptr<ResourceData> Shader::createResource(const std::string _vertexShaderPath, const std::string _fragmentShaderPath)
		{
			File vertexShaderFile;
			File fragmentShaderFile;
			if (!readFile(vertexShaderFile, _vertexShaderPath))
			{
				log::error("Cannot reload shader, file read failed! (" + _vertexShaderPath + ")");
				return nullptr;
			}
			if (!readFile(fragmentShaderFile, _fragmentShaderPath))
			{
				log::error("Cannot reload shader, file read failed! (" + _fragmentShaderPath + ")");
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

		void Shader::create(const std::string_view _vertexShaderPath, const std::string_view _fragmentShaderPath, ResourceLoader _resourceLoader)
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

		void Shader::extractUniforms(std::vector<std::shared_ptr<Uniform>>& _uniforms)
		{
			se_assert(resourceData);

			static constexpr size_t MAX_UNIFORMS = 256;
			bgfx::UniformHandle uniformHandles[MAX_UNIFORMS];

			bgfx::ShaderHandle vertexShaderHandle = { resourceData->vertexShaderHandle };
			const uint16_t numVertexUniforms = bgfx::getShaderUniforms(vertexShaderHandle, uniformHandles, MAX_UNIFORMS);
			se_assert(numVertexUniforms <= MAX_UNIFORMS);
			for (size_t i = 0; i < (size_t)numVertexUniforms; i++)
			{
				bgfx::UniformInfo info;
				bgfx::getUniformInfo(uniformHandles[i], info);

				auto it = std::find_if(_uniforms.begin(), _uniforms.end(), [&](const std::shared_ptr<Uniform>& _uniform) { return _uniform->getName() == info.name; });
				if (it != _uniforms.end())
				{
					continue;
				}

				bgfx::UniformHandle newHandle = bgfx::createUniform(info.name, info.type);
				_uniforms.emplace_back(std::make_shared<Uniform>(info, newHandle));
			}

			bgfx::ShaderHandle fragmentShaderHandle = { resourceData->fragmentShaderHandle };
			const uint16_t numFragmentUniforms = bgfx::getShaderUniforms(fragmentShaderHandle, uniformHandles, MAX_UNIFORMS);
			se_assert(numFragmentUniforms <= MAX_UNIFORMS);
			for (size_t i = 0; i < (size_t)numFragmentUniforms; i++)
			{
				bgfx::UniformInfo info;
				bgfx::getUniformInfo(uniformHandles[i], info);

				auto it = std::find_if(_uniforms.begin(), _uniforms.end(), [&](const std::shared_ptr<Uniform>& _uniform) { return _uniform->getName() == info.name; });
				if (it != _uniforms.end())
				{
					continue;
				}

				bgfx::UniformHandle newHandle = bgfx::createUniform(info.name, info.type);
				_uniforms.emplace_back(std::make_shared<Uniform>(info, newHandle));
			}
		}
	}
}
