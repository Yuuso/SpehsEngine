#pragma once

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Graphics/Internal/BufferObject.h"
#include "SpehsEngine/Graphics/VertexAttribute.h"
#include <stdint.h>
#include <vector>


namespace se
{
	namespace graphics
	{
		class VertexBuffer : public BufferObject
		{
		public:

			VertexBuffer();
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


			#define get_decl(__attribute) \
				template <VertexAttribute::VertexAttributeFlag _attribute> \
				typename std::enable_if<_attribute == VertexAttribute::VertexAttributeFlag::__attribute, VertexAttribute::##__attribute##Type&>::type \
				get(const size_t _at) \
				{ \
					se_assert(checkBit(attributes, _attribute)); \
					bufferChanged = true; \
					return *(VertexAttribute::##__attribute##Type*)&buffer[_at * vertexBytes + offset<_attribute>()]; \
				} \
				template <VertexAttribute::VertexAttributeFlag _attribute> \
				typename std::enable_if<_attribute == VertexAttribute::VertexAttributeFlag::__attribute, const VertexAttribute::##__attribute##Type&>::type \
				get(const size_t _at) const \
				{ \
					se_assert(checkBit(attributes, _attribute)); \
					return *(const VertexAttribute::##__attribute##Type*)&buffer[_at * vertexBytes + offset<_attribute>()]; \
				} \

			get_decl(Position)
			get_decl(Normal)
			get_decl(Tangent)
			get_decl(Bitangent)
			get_decl(Color0)
			get_decl(Color1)
			get_decl(Color2)
			get_decl(Color3)
			get_decl(Weight)
			get_decl(Indices)
			get_decl(TexCoord0)
			get_decl(TexCoord1)
			get_decl(TexCoord2)
			get_decl(Data0)
			get_decl(Data1)
			get_decl(Data2)
			get_decl(Data3)
			get_decl(Data4)

			#undef get_decl

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
}
