#include "stdafx.h"
#include "SpehsEngine/Graphics/Shader.h"

#include "SpehsEngine/Core/File/File.h"
#include "SpehsEngine/Core/Log.h"


namespace se
{
	namespace graphics
	{
		Shader::Shader(const std::string_view _name)
			: name(_name)
			, programHandle(BGFX_INVALID_HANDLE)
			, vertexShaderHandle(BGFX_INVALID_HANDLE)
			, fragmentShaderHandle(BGFX_INVALID_HANDLE)
		{
		}
		Shader::~Shader()
		{
			destroy();
		}

		void Shader::destroy()
		{
			if (bgfx::isValid(programHandle))
			{
				bgfx::destroy(programHandle);
				programHandle = BGFX_INVALID_HANDLE;
			}
			if (bgfx::isValid(vertexShaderHandle))
			{
				bgfx::destroy(vertexShaderHandle);
				vertexShaderHandle = BGFX_INVALID_HANDLE;
			}
			if (bgfx::isValid(fragmentShaderHandle))
			{
				bgfx::destroy(fragmentShaderHandle);
				fragmentShaderHandle = BGFX_INVALID_HANDLE;
			}
		}
		void Shader::reload()
		{
			if (vertexShaderPath.empty() || fragmentShaderPath.empty())
			{
				log::error("Cannot reload shader, path not defined!");
				return;
			}
			File vertexShaderFile;
			File fragmentShaderFile;
			if (!readFile(vertexShaderFile, vertexShaderPath))
			{
				log::error("Cannot reload shader, file read failed! (" + vertexShaderPath + ")");
				return;
			}
			if (!readFile(fragmentShaderFile, fragmentShaderPath))
			{
				log::error("Cannot reload shader, file read failed! (" + fragmentShaderPath + ")");
				return;
			}
			const bgfx::Memory* vertexBuffer = bgfx::copy(vertexShaderFile.data.data(), uint32_t(vertexShaderFile.data.size()));
			const bgfx::Memory* fragmentBuffer = bgfx::copy(fragmentShaderFile.data.data(), uint32_t(fragmentShaderFile.data.size()));
			bgfx::ShaderHandle vertexShader = bgfx::createShader(vertexBuffer);
			bgfx::ShaderHandle fragmentShader = bgfx::createShader(fragmentBuffer);
			create(vertexShader, fragmentShader);
		}
		void Shader::create(const std::string_view _vertexShaderPath, const std::string_view _fragmentShaderPath)
		{
			vertexShaderPath = _vertexShaderPath;
			fragmentShaderPath = _fragmentShaderPath;
			reload();
		}
		void Shader::create(bgfx::ShaderHandle _vertexShader, bgfx::ShaderHandle _fragmentShader)
		{
			destroy();

			vertexShaderHandle = _vertexShader;
			fragmentShaderHandle = _fragmentShader;

			programHandle = bgfx::createProgram(vertexShaderHandle, fragmentShaderHandle, false);
			if (!bgfx::isValid(programHandle))
			{
				log::error("Failed to create shader program! (" + name + ")");
			}
		}
		void Shader::extractUniforms(std::vector<std::shared_ptr<Uniform>>& _uniforms)
		{
			static constexpr size_t MAX_UNIFORMS = 256;
			bgfx::UniformHandle uniformHandles[MAX_UNIFORMS];

			const uint16_t numVertexUniforms = bgfx::getShaderUniforms(vertexShaderHandle, uniformHandles, MAX_UNIFORMS);
			se_assert(numVertexUniforms <= MAX_UNIFORMS);
			for (size_t i = 0; i < (size_t)numVertexUniforms; i++)
			{
				bgfx::UniformInfo info;
				bgfx::getUniformInfo(uniformHandles[i], info);

				auto it = std::find_if(_uniforms.begin(), _uniforms.end(), [&](const std::shared_ptr<Uniform>& _uniform) { return _uniform->getName() == info.name; });
				if (it != _uniforms.end())
				{
					bgfx::destroy(uniformHandles[i]);
					continue;
				}

				_uniforms.emplace_back(std::make_shared<Uniform>(info, uniformHandles[i]));
			}

			const uint16_t numFragmentUniforms = bgfx::getShaderUniforms(fragmentShaderHandle, uniformHandles, MAX_UNIFORMS);
			se_assert(numFragmentUniforms <= MAX_UNIFORMS);
			for (size_t i = 0; i < (size_t)numFragmentUniforms; i++)
			{
				bgfx::UniformInfo info;
				bgfx::getUniformInfo(uniformHandles[i], info);

				auto it = std::find_if(_uniforms.begin(), _uniforms.end(), [&](const std::shared_ptr<Uniform>& _uniform) { return _uniform->getName() == info.name; });
				if (it != _uniforms.end())
				{
					bgfx::destroy(uniformHandles[i]);
					continue;
				}

				_uniforms.emplace_back(std::make_shared<Uniform>(info, uniformHandles[i]));
			}
		}

		const std::string& Shader::getName() const
		{
			return name;
		}
	}
}
