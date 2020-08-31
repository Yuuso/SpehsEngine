#include "stdafx.h"
#include "SpehsEngine/Graphics/Uniform.h"

#include "SpehsEngine/Graphics/Texture.h"


namespace se
{
	namespace graphics
	{
		Uniform::Uniform(const bgfx::UniformInfo& _uniformInfo, const bgfx::UniformHandle _uniformHandle)
			: uniformHandle(BGFX_INVALID_HANDLE)
		{
			reset(_uniformInfo, _uniformHandle);
		}
		Uniform::~Uniform()
		{
			if (bgfx::isValid(uniformHandle))
			{
				bgfx::destroy(uniformHandle);
				uniformHandle = BGFX_INVALID_HANDLE;
			}
		}

		void Uniform::reset(const bgfx::UniformInfo& _uniformInfo, const bgfx::UniformHandle _uniformHandle)
		{
			if (bgfx::isValid(uniformHandle))
			{
				bgfx::destroy(uniformHandle);
				uniformHandle = BGFX_INVALID_HANDLE;
			}

			name = _uniformInfo.name;
			numElements = _uniformInfo.num;
			switch (_uniformInfo.type)
			{
				default: se_assert(false); break;
				case bgfx::UniformType::Sampler:
				{
					type = UniformType::Sampler;
					break;
				}
				case bgfx::UniformType::Vec4:
				{
					type = UniformType::Vec4;
					break;
				}
				case bgfx::UniformType::Mat3:
				{
					type = UniformType::Mat3;
					break;
				}
				case bgfx::UniformType::Mat4:
				{
					type = UniformType::Mat4;
					break;
				}
			}
			uniformHandle = _uniformHandle;
		}

		bool Uniform::operator==(const Uniform& _other) const
		{
			const bool equals = name == _other.name;
			se_assert(!equals || (type == _other.type && numElements == _other.numElements));
			return equals;
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

		void Uniform::set(const glm::vec4& _value, const uint16_t _numElements)
		{
			se_assert(type == UniformType::Vec4);
			se_assert(bgfx::isValid(uniformHandle));
			se_assert(_numElements <= numElements);
			bgfx::setUniform(uniformHandle, reinterpret_cast<const void*>(&_value), _numElements);
		}
		void Uniform::set(const glm::mat3& _value, const uint16_t _numElements)
		{
			se_assert(type == UniformType::Mat3);
			se_assert(bgfx::isValid(uniformHandle));
			se_assert(_numElements <= numElements);
			bgfx::setUniform(uniformHandle, reinterpret_cast<const void*>(&_value), _numElements);
		}
		void Uniform::set(const glm::mat4& _value, const uint16_t _numElements)
		{
			se_assert(type == UniformType::Mat4);
			se_assert(bgfx::isValid(uniformHandle));
			se_assert(_numElements <= numElements);
			bgfx::setUniform(uniformHandle, reinterpret_cast<const void*>(&_value), _numElements);
		}
		void Uniform::set(const float* _value, const uint16_t _numElements)
		{
			se_assert(_value != nullptr);
			se_assert(_numElements > 0);
			se_assert(bgfx::isValid(uniformHandle));
			switch (type)
			{
				case se::graphics::UniformType::Sampler:
				{
					se_assert_m(false, "UniformType::Sampler is not allowed here!");
					break;
				}
				case se::graphics::UniformType::Vec4:
				{
					se_assert(_numElements % 4 == 0);
					se_assert((_numElements / 4) <= numElements);
					bgfx::setUniform(uniformHandle, reinterpret_cast<const void*>(_value), _numElements / 4);
					break;
				}
				case se::graphics::UniformType::Mat3:
				{
					se_assert(_numElements % 9 == 0);
					se_assert((_numElements / 9) <= numElements);
					bgfx::setUniform(uniformHandle, reinterpret_cast<const void*>(_value), _numElements / 9);
					break;
				}
				case se::graphics::UniformType::Mat4:
				{
					se_assert(_numElements % 16 == 0);
					se_assert((_numElements / 16) <= numElements);
					bgfx::setUniform(uniformHandle, reinterpret_cast<const void*>(_value), _numElements / 16);
					break;
				}
			}
		}
		void Uniform::set(const std::shared_ptr<Texture>& _value, const uint8_t _stage)
		{
			se_assert(_value);
			bgfx::setTexture(_stage, uniformHandle, { _value->getHandle() });
		}
	}
}
