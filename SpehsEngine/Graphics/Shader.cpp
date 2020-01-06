#include "stdafx.h"
#include "SpehsEngine/Graphics/Shader.h"


namespace se
{
	namespace graphics
	{
		Shader::Shader(const std::string_view _name)
			: name(_name)
		{
			programHandle.idx = bgfx::kInvalidHandle;
		}
		Shader::~Shader()
		{
			if (bgfx::isValid(programHandle))
				bgfx::destroy(programHandle);
		}

		bool Shader::load(const std::string_view /*_vertexShader*/, const std::string_view /*_fragmentShader*/)
		{
			return false;
		}
		bool Shader::load(const bgfx::ProgramHandle _programHandle)
		{
			programHandle = _programHandle;
			return true;
		}

		const std::string& Shader::getName() const
		{
			return name;
		}
	}
}
