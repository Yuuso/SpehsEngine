#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/DefaultUniforms.h"

#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Graphics/Renderer.h"

#include <chrono>


namespace se
{
	namespace graphics
	{
		DefaultUniforms::DefaultUniforms()
			: normal(BGFX_INVALID_HANDLE)
		{
			normal = bgfx::createUniform("u_normal", bgfx::UniformType::Mat4, SE_MAX_BONES);
			primitiveColor = bgfx::createUniform("u_primitiveColor", bgfx::UniformType::Vec4);
		}
		DefaultUniforms::~DefaultUniforms()
		{
			destroy();
		}

		void DefaultUniforms::destroy()
		{
			if (bgfx::isValid(normal))
			{
				bgfx::destroy(normal);
				normal = BGFX_INVALID_HANDLE;
			}
			if (bgfx::isValid(primitiveColor))
			{
				bgfx::destroy(primitiveColor);
				primitiveColor = BGFX_INVALID_HANDLE;
			}
		}

		void DefaultUniforms::setNormalMatrices(const UniformMatrices& _normal)
		{
			bgfx::setUniform(normal, reinterpret_cast<const void*>(_normal.data()), static_cast<uint16_t>(_normal.size()));
		}

		void DefaultUniforms::setPrimitiveColor(const Color& _color)
		{
			static_assert(sizeof(Color) == sizeof(glm::vec4));
			bgfx::setUniform(primitiveColor, reinterpret_cast<const void*>(&_color));
		}
	}
}
