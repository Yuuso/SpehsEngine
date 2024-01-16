#pragma once

#include "SpehsEngine/Graphics/BufferObject.h"
#include "SpehsEngine/Graphics/VertexAttribute.h"


namespace se::gfx
{
	class VertexBuffer : public BufferObject
	{
	public:

		VertexBuffer() = default;
		VertexBuffer(const VertexAttributeFlagsType _vertexAttributes);
		~VertexBuffer();

		VertexBuffer(const VertexBuffer& _other);
		VertexBuffer& operator=(const VertexBuffer& _other);

		VertexBuffer(VertexBuffer&& _other);
		VertexBuffer& operator=(VertexBuffer&& _other);


		void updateBuffer() override;

		const VertexAttributeFlagsType getAttributes() const;
		void setAttributes(const VertexAttributeFlagsType _vertexAttributes);

		const size_t size() const;
		const size_t bytes() const;
		void resize(const size_t _size);
		void grow(const size_t _amount);
		void pushBack(const VertexBuffer& _vertices);
		void erase(const size_t _begin, const size_t _end);
		void clear();
		const void* data() const;


		#define IMPL_GET_FUNCS(__attribute) \
			template <VertexAttribute::VertexAttributeFlag _attribute> \
			typename std::enable_if<_attribute == VertexAttribute::VertexAttributeFlag::__attribute, VertexAttribute::##__attribute##Type&>::type \
			get(const size_t _at) \
			{ \
				se_assert(checkBit(attributes, _attribute)); \
				se_assert(size() > _at); \
				bufferChanged = true; \
				return *reinterpret_cast<VertexAttribute::##__attribute##Type*>(&buffer[_at * vertexBytes + offset<_attribute>()]); \
			} \
			template <VertexAttribute::VertexAttributeFlag _attribute> \
			typename std::enable_if<_attribute == VertexAttribute::VertexAttributeFlag::__attribute, const VertexAttribute::##__attribute##Type&>::type \
			get(const size_t _at) const \
			{ \
				se_assert(checkBit(attributes, _attribute)); \
				se_assert(size() > _at); \
				return *reinterpret_cast<const VertexAttribute::##__attribute##Type*>(&buffer[_at * vertexBytes + offset<_attribute>()]); \
			}
		IMPL_GET_FUNCS(Position)
		IMPL_GET_FUNCS(Normal)
		IMPL_GET_FUNCS(Tangent)
		IMPL_GET_FUNCS(Bitangent)
		IMPL_GET_FUNCS(Color0)
		IMPL_GET_FUNCS(Color1)
		IMPL_GET_FUNCS(Color2)
		IMPL_GET_FUNCS(Color3)
		IMPL_GET_FUNCS(Weight)
		IMPL_GET_FUNCS(Indices)
		IMPL_GET_FUNCS(TexCoord0)
		IMPL_GET_FUNCS(TexCoord1)
		IMPL_GET_FUNCS(TexCoord2)
		IMPL_GET_FUNCS(Data0)
		IMPL_GET_FUNCS(Data1)
		IMPL_GET_FUNCS(Data2)
		IMPL_GET_FUNCS(Data3)
		IMPL_GET_FUNCS(Data4)
		#undef IMPL_GET_FUNCS

		// Set if attribute flag is set, return false if attribute is not set
		template <VertexAttribute::VertexAttributeFlag _attribute, typename T>
		bool set(size_t _at, const T& _value)
		{
			if (!checkBit(attributes, _attribute))
				return false;
			get<_attribute>(_at) = _value;
			return true;
		}

	private:

		// TODO: cache offset?
		template<VertexAttribute::VertexAttributeFlag _attribute>
		size_t offset() const
		{
			size_t result = 0;

			#define CHECK_ATTR_OFFSET(ATTR) \
				if (checkBit(attributes, VertexAttribute::VertexAttributeFlag::ATTR)) \
				{ \
					if constexpr (_attribute == VertexAttribute::VertexAttributeFlag::ATTR) \
						return result; \
					else \
						result += sizeof(VertexAttribute::##ATTR##Type); \
				}
			CHECK_ATTR_OFFSET(Position)
			CHECK_ATTR_OFFSET(Normal)
			CHECK_ATTR_OFFSET(Tangent)
			CHECK_ATTR_OFFSET(Bitangent)
			CHECK_ATTR_OFFSET(Color0)
			CHECK_ATTR_OFFSET(Color1)
			CHECK_ATTR_OFFSET(Color2)
			CHECK_ATTR_OFFSET(Color3)
			CHECK_ATTR_OFFSET(Weight)
			CHECK_ATTR_OFFSET(Indices)
			CHECK_ATTR_OFFSET(TexCoord0)
			CHECK_ATTR_OFFSET(TexCoord1)
			CHECK_ATTR_OFFSET(TexCoord2)
			CHECK_ATTR_OFFSET(Data0)
			CHECK_ATTR_OFFSET(Data1)
			CHECK_ATTR_OFFSET(Data2)
			CHECK_ATTR_OFFSET(Data3)
			CHECK_ATTR_OFFSET(Data4)
			#undef CHECK_ATTR_OFFSET

			return result;
		}
		void calculateVertexSize();

		std::vector<uint8_t> buffer;
		VertexAttributeFlagsType attributes = 0;
		size_t vertexBytes = 0;
	};
}
