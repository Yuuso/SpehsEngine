#pragma once


namespace se
{
	namespace graphics
	{
		typedef uint32_t VertexAttributeFlagsType;

		namespace VertexAttribute
		{
			enum VertexAttributeFlag : VertexAttributeFlagsType
			{
				Position		= (1 << 0),
				Normal			= (1 << 1),
				Tangent			= (1 << 2),
				Bitangent		= (1 << 3),
				Color0			= (1 << 4),
				Color1			= (1 << 5),
				Color2			= (1 << 6),
				Color3			= (1 << 7),
				Weight			= (1 << 9),
				Indices			= (1 << 10),
				TexCoord0		= (1 << 11),
				TexCoord1		= (1 << 12),
				TexCoord2		= (1 << 13),
				Data0			= (1 << 14),
				Data1			= (1 << 15),
				Data2			= (1 << 16),
				Data3			= (1 << 17),
				Data4			= (1 << 18),
			};

			typedef glm::vec3  PositionType;
			typedef glm::vec3  NormalType;
			typedef glm::vec3  TangentType;
			typedef glm::vec3  BitangentType;
			typedef se::Color  Color0Type;
			typedef se::Color  Color1Type;
			typedef se::Color  Color2Type;
			typedef se::Color  Color3Type;
			typedef glm::vec4  WeightType;
			typedef glm::i16vec4 IndicesType;
			typedef glm::vec2  TexCoord0Type;
			typedef glm::vec2  TexCoord1Type;
			typedef glm::vec2  TexCoord2Type;
			typedef glm::vec4  Data0Type;
			typedef glm::vec4  Data1Type;
			typedef glm::vec4  Data2Type;
			typedef glm::vec4  Data3Type;
			typedef glm::vec4  Data4Type;
		}
	}
}
