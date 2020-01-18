#pragma once

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Graphics/Types.h"
#include <stdint.h>
#include <vector>


namespace se
{
	namespace graphics
	{
		class VertexBuffer
		{
		public:

			VertexBuffer();
			VertexBuffer(const VertexAttributeFlagsType _vertexAttributes);


			const VertexAttributeFlagsType getAttributes() const;
			void setAttributes(const VertexAttributeFlagsType _vertexAttributes);

			const size_t size() const;
			const size_t bytes() const;
			void resize(const size_t _size);
			void pushBack(const VertexBuffer& _vertices);
			void erase(const size_t _begin, const size_t _end);
			void clear();
			const void* data() const;


			#define get_decl(__attribute, __returntype) \
				template <VertexAttributeFlag _attribute> \
				typename std::enable_if<_attribute == __attribute, __returntype&>::type \
				get(const size_t _at) \
				{ \
					se_assert(checkBit(attributes, _attribute)); \
					return *(__returntype*)&buffer[_at * vertexBytes + offset<_attribute>()]; \
				} \
				template <VertexAttributeFlag _attribute> \
				typename std::enable_if<_attribute == __attribute, const __returntype&>::type \
				get(const size_t _at) const \
				{ \
					se_assert(checkBit(attributes, _attribute)); \
					return *(const __returntype*)&buffer[_at * vertexBytes + offset<_attribute>()]; \
				} \

			get_decl(VertexAttributeFlag::Position,		glm::vec3)
			get_decl(VertexAttributeFlag::Normal,		glm::vec3)
			get_decl(VertexAttributeFlag::Tangent,		glm::vec3)
			get_decl(VertexAttributeFlag::Bitangent,	glm::vec3)
			get_decl(VertexAttributeFlag::Color0,		Color)
			get_decl(VertexAttributeFlag::Color1,		Color)
			get_decl(VertexAttributeFlag::Color2,		Color)
			get_decl(VertexAttributeFlag::Color3,		Color)
			get_decl(VertexAttributeFlag::Weight,		float)
			get_decl(VertexAttributeFlag::TexCoord0,	glm::vec2)
			get_decl(VertexAttributeFlag::TexCoord1,	glm::vec2)
			get_decl(VertexAttributeFlag::TexCoord2,	glm::vec2)
			get_decl(VertexAttributeFlag::TexCoord3,	glm::vec2)
			get_decl(VertexAttributeFlag::TexCoord4,	glm::vec2)
			get_decl(VertexAttributeFlag::TexCoord5,	glm::vec2)
			get_decl(VertexAttributeFlag::TexCoord6,	glm::vec2)
			get_decl(VertexAttributeFlag::TexCoord7,	glm::vec2)

		private:

			// TODO: cache offset?
			template<VertexAttributeFlag _attribute>
			size_t offset() const
			{
				size_t result = 0;
				if(checkBit(attributes, Position))	{ if constexpr (_attribute == Position)	 return result;	else result += sizeof(glm::vec3); }
				if(checkBit(attributes, Normal))	{ if constexpr (_attribute == Normal)	 return result;	else result += sizeof(glm::vec3); }
				if(checkBit(attributes, Tangent))	{ if constexpr (_attribute == Tangent)	 return result;	else result += sizeof(glm::vec3); }
				if(checkBit(attributes, Bitangent))	{ if constexpr (_attribute == Bitangent) return result;	else result += sizeof(glm::vec3); }
				if(checkBit(attributes, Color0))	{ if constexpr (_attribute == Color0)	 return result;	else result += sizeof(Color)	; }
				if(checkBit(attributes, Color1))	{ if constexpr (_attribute == Color1)	 return result;	else result += sizeof(Color)	; }
				if(checkBit(attributes, Color2))	{ if constexpr (_attribute == Color2)	 return result;	else result += sizeof(Color)	; }
				if(checkBit(attributes, Color3))	{ if constexpr (_attribute == Color3)	 return result;	else result += sizeof(Color)	; }
				if(checkBit(attributes, Weight))	{ if constexpr (_attribute == Weight)	 return result;	else result += sizeof(float)	; }
				if(checkBit(attributes, TexCoord0))	{ if constexpr (_attribute == TexCoord0) return result;	else result += sizeof(glm::vec2); }
				if(checkBit(attributes, TexCoord1))	{ if constexpr (_attribute == TexCoord1) return result;	else result += sizeof(glm::vec2); }
				if(checkBit(attributes, TexCoord2))	{ if constexpr (_attribute == TexCoord2) return result;	else result += sizeof(glm::vec2); }
				if(checkBit(attributes, TexCoord3))	{ if constexpr (_attribute == TexCoord3) return result;	else result += sizeof(glm::vec2); }
				if(checkBit(attributes, TexCoord4))	{ if constexpr (_attribute == TexCoord4) return result;	else result += sizeof(glm::vec2); }
				if(checkBit(attributes, TexCoord5))	{ if constexpr (_attribute == TexCoord5) return result;	else result += sizeof(glm::vec2); }
				if(checkBit(attributes, TexCoord6))	{ if constexpr (_attribute == TexCoord6) return result;	else result += sizeof(glm::vec2); }
				if(checkBit(attributes, TexCoord7))	{ if constexpr (_attribute == TexCoord7) return result;	else result += sizeof(glm::vec2); }
				return result;
			}
			void calculateVertexSize();

			std::vector<uint8_t> buffer;
			VertexAttributeFlagsType attributes = 0;
			size_t vertexBytes = 0;
		};
	}
}
