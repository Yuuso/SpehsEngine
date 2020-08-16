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
		{
		}
		Shader::~Shader()
		{
			destroy();
		}

		void Shader::destroy()
		{
			uniforms.clear();
			if (bgfx::isValid(programHandle))
			{
				bgfx::destroy(programHandle);
				programHandle = BGFX_INVALID_HANDLE;
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
			if (bgfx::isValid(programHandle))
			{
				bgfx::destroy(programHandle);
				programHandle = BGFX_INVALID_HANDLE;
			}

			programHandle = bgfx::createProgram(_vertexShader, _fragmentShader, true);
		}

		const std::string& Shader::getName() const
		{
			return name;
		}
	}
}
