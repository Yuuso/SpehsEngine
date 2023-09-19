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
			} \

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

	private:

		// TODO: cache offset?
		template<VertexAttribute::VertexAttributeFlag _attribute>
		size_t offset() const
		{
			size_t result = 0;

			#define check_attribute_offset(__attribute) \
				if (checkBit(attributes, VertexAttribute::VertexAttributeFlag::__attribute)) { if constexpr (_attribute == VertexAttribute::VertexAttributeFlag::__attribute) return result; else result += sizeof(VertexAttribute::##__attribute##Type); }

			check_attribute_offset(Position)
			check_attribute_offset(Normal)
			check_attribute_offset(Tangent)
			check_attribute_offset(Bitangent)
			check_attribute_offset(Color0)
			check_attribute_offset(Color1)
			check_attribute_offset(Color2)
			check_attribute_offset(Color3)
			check_attribute_offset(Weight)
			check_attribute_offset(Indices)
			check_attribute_offset(TexCoord0)
			check_attribute_offset(TexCoord1)
			check_attribute_offset(TexCoord2)
			check_attribute_offset(Data0)
			check_attribute_offset(Data1)
			check_attribute_offset(Data2)
			check_attribute_offset(Data3)
			check_attribute_offset(Data4)

			#undef check_attribute_offset

			return result;
		}
		void calculateVertexSize();

		std::vector<uint8_t> buffer;
		VertexAttributeFlagsType attributes = 0;
		size_t vertexBytes = 0;
	};
}
