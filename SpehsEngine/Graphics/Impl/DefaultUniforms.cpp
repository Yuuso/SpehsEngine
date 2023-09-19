#include "stdafx.h"
#include "SpehsEngine/Graphics/Impl/DefaultUniforms.h"

#include "SpehsEngine/Graphics/Renderer.h"


DefaultUniforms::DefaultUniforms()
	: normal(BGFX_INVALID_HANDLE)
{
	normal = bgfx::createUniform("u_normal", bgfx::UniformType::Mat4, SE_MAX_BONES);
	primitiveColor = bgfx::createUniform("u_primitiveColor", bgfx::UniformType::Vec4);
	billboardInfo = bgfx::createUniform("u_billboardInfo", bgfx::UniformType::Vec4);
	engineUtility = bgfx::createUniform("u_engineUtility", bgfx::UniformType::Vec4);
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
	if (bgfx::isValid(billboardInfo))
	{
		bgfx::destroy(billboardInfo);
		billboardInfo = BGFX_INVALID_HANDLE;
	}
	if (bgfx::isValid(engineUtility))
	{
		bgfx::destroy(engineUtility);
		engineUtility = BGFX_INVALID_HANDLE;
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

void DefaultUniforms::setBillboardInfo(const glm::vec3& _position, const bool _cylindrical)
{
	const glm::vec4 data = glm::vec4(_position.x, _position.y, _position.z, _cylindrical ? 1.0f : 0.0f);
	bgfx::setUniform(billboardInfo, reinterpret_cast<const void*>(&data));
}

void DefaultUniforms::updateEngineUtility()
{
	const glm::vec4 data = glm::vec4(se::time::getRunTime().asSeconds<float>(), 0.0f, 0.0f, 0.0f);
	bgfx::setUniform(engineUtility, reinterpret_cast<const void*>(&data));
}
