#include "stdafx.h"
#include "SpehsEngine/Graphics/Uniform.h"

#include "SpehsEngine/Graphics/Font.h"
#include "SpehsEngine/Graphics/Texture.h"


namespace se
{
	namespace graphics
	{
		Uniform::Uniform(const std::string_view _name, const UniformType _type, const uint16_t _numElements)
			: name(_name)
			, type(_type)
			, numElements(_numElements)
		{
			const bgfx::UniformHandle bgfxUniformHandle = bgfx::createUniform(name.c_str(), getUniformType(type), numElements);
			uniformHandle = bgfxUniformHandle.idx;
			if (_numElements > 1)
			{
				// Verify that the number of elements is correct.
				bgfx::UniformInfo uniformInfo;
				bgfx::getUniformInfo(bgfxUniformHandle, uniformInfo);
				if (uniformInfo.num != _numElements)
				{
					se::log::error("Uniform creation failed. Mismatching number of elements. Hardware/API limit reached?");
				}
			}
		}
		Uniform::~Uniform()
		{
			safeDestroy<bgfx::UniformHandle>(uniformHandle);
		}

		bool Uniform::operator==(const Uniform& _other) const
		{
			return uniformHandle == _other.uniformHandle;
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
			bgfx::setUniform({ uniformHandle }, reinterpret_cast<const void*>(_value), _numElements);
		}
		void Uniform::set(const glm::mat3* _value, const uint16_t _numElements)
		{
			se_assert(_value != nullptr);
			se_assert(type == UniformType::Mat3);
			se_assert(_numElements <= numElements);
			bgfx::setUniform({ uniformHandle }, reinterpret_cast<const void*>(_value), _numElements);
		}
		void Uniform::set(const glm::mat4* _value, const uint16_t _numElements)
		{
			se_assert(_value != nullptr);
			se_assert(type == UniformType::Mat4);
			se_assert(_numElements <= numElements);
			bgfx::setUniform({ uniformHandle }, reinterpret_cast<const void*>(_value), _numElements);
		}
		void Uniform::set(const Texture& _value, const uint8_t _stage)
		{
			bgfx::setTexture(_stage, { uniformHandle }, { _value.getHandle() });
		}
		void Uniform::set(const Font& _value, const uint8_t _stage)
		{
			bgfx::setTexture(_stage, { uniformHandle }, { _value.getHandle() });
		}
	}
}
