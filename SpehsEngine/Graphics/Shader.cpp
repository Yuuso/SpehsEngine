#include "stdafx.h"
#include "SpehsEngine/Graphics/Shader.h"

#include "SpehsEngine/Core/File/File.h"


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
			readFile(vertexShaderFile, vertexShaderPath);
			readFile(fragmentShaderFile, fragmentShaderPath);
			const bgfx::Memory* vertexBuffer = bgfx::copy(vertexShaderFile.data.data(), vertexShaderFile.data.size());
			const bgfx::Memory* fragmentBuffer = bgfx::copy(fragmentShaderFile.data.data(), fragmentShaderFile.data.size());
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

			static constexpr size_t MAX_UNIFORMS = 256;
			bgfx::UniformHandle temp[MAX_UNIFORMS];

			const uint16_t numVertexUniforms = bgfx::getShaderUniforms(_vertexShader, temp, sizeof(temp));
			se_assert(numVertexUniforms <= MAX_UNIFORMS);
			for (size_t i = 0; i < (size_t)numVertexUniforms; i++)
			{
				bgfx::UniformInfo info;
				bgfx::getUniformInfo(temp[i], info);

				bool oldFound = false;
				for (auto& uniform : uniforms)
				{
					if (uniform->getName() == info.name)
					{
						uniform->reset(info, temp[i]);
						oldFound = true;
						break;
					}
				}
				if (oldFound)
					continue;

				uniforms.emplace_back(std::make_shared<Uniform>(info, temp[i]));
			}

			const uint16_t numFragmentUniforms = bgfx::getShaderUniforms(_fragmentShader, temp, sizeof(temp));
			se_assert(numFragmentUniforms <= MAX_UNIFORMS);
			for (size_t i = 0; i < (size_t)numFragmentUniforms; i++)
			{
				bgfx::UniformInfo info;
				bgfx::getUniformInfo(temp[i], info);

				// TODO: Skip duplicates from vertex shader
				//bgfx::destroy(temp[i]);
				//temp[i] = BGFX_INVALID_HANDLE;

				bool oldFound = false;
				for (auto& uniform : uniforms)
				{
					if (uniform->getName() == info.name)
					{
						uniform->reset(info, temp[i]);
						oldFound = true;
						break;
					}
				}
				if (oldFound)
					continue;

				uniforms.emplace_back(std::make_shared<Uniform>(info, temp[i]));
			}
		}

		const std::string& Shader::getName() const
		{
			return name;
		}
	}
}
