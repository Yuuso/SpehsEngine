#include "stdafx.h"
#include "SpehsEngine/Graphics/Uniform.h"

#include "SpehsEngine/Graphics/Font.h"
#include "SpehsEngine/Graphics/Texture.h"


Uniform::Uniform(const std::string_view _name, const UniformType _type, const uint16_t _numElements)
	: name(_name)
	, type(_type)
	, numElements(_numElements)
{
	uniformHandle = bgfx::createUniform(name.c_str(), getUniformType(type), numElements).idx;
}
Uniform::~Uniform()
{
	safeDestroy<bgfx::UniformHandle>(uniformHandle);
}

bool Uniform::operator==(const Uniform& _other) const
{
	return uniformHandle.u16handle == _other.uniformHandle.u16handle;
}

const UniformType Uniform::getType() const
{
	return type;
}
const uint16_t Uniform::getNumElements() const
{
	return numElements;
}
const std::string& Uniform::getName() const
{
	return name;
}

void Uniform::set(const glm::vec4* _value, const uint16_t _numElements)
{
	se_assert(_value != nullptr);
	se_assert(type == UniformType::Vec4);
	se_assert(_numElements <= numElements);
	bgfx::setUniform({ uniformHandle.u16handle }, reinterpret_cast<const void*>(_value), _numElements);
}
void Uniform::set(const glm::mat3* _value, const uint16_t _numElements)
{
	se_assert(_value != nullptr);
	se_assert(type == UniformType::Mat3);
	se_assert(_numElements <= numElements);
	bgfx::setUniform({ uniformHandle.u16handle }, reinterpret_cast<const void*>(_value), _numElements);
}
void Uniform::set(const glm::mat4* _value, const uint16_t _numElements)
{
	se_assert(_value != nullptr);
	se_assert(type == UniformType::Mat4);
	se_assert(_numElements <= numElements);
	bgfx::setUniform({ uniformHandle.u16handle }, reinterpret_cast<const void*>(_value), _numElements);
}
void Uniform::set(const Texture& _value, const uint8_t _stage)
{
	bgfx::setTexture(_stage, { uniformHandle.u16handle }, { _value.getHandle().u16handle });
}
void Uniform::set(const Font& _value, const uint8_t _stage)
{
	bgfx::setTexture(_stage, { uniformHandle.u16handle }, { _value.getHandle().u16handle });
}
