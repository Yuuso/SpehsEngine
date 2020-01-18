#pragma once

#include "SpehsEngine/Core/Color.h"
#include "bgfx/bgfx.h" // TODO: get rid of this
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include <stdint.h>
#include <numeric>

// TODO: Separate all or at least separate internal


namespace se
{
	namespace graphics
	{
		typedef uint16_t IndexType;

		typedef uint32_t RenderFlagsType;
		enum RenderFlag : RenderFlagsType
		{
			CullFrontFace		= (1 << 0),
			CullBackFace		= (1 << 1),
			DepthTest			= (1 << 2),
		};

		typedef uint32_t RendererFlagsType;
		enum RendererFlag : RendererFlagsType
		{
			CWFrontFace			= (1 << 0),
			MSAA2				= (1 << 1),
			MSAA4				= (1 << 2),
			MSAA8				= (1 << 3),
			MSAA16				= (1 << 4),
			VSync				= (1 << 5),
			Direct3D9			= (1 << 6),
			Direct3D11			= (1 << 7),
			Direct3D12			= (1 << 8),
			Gnm					= (1 << 9),
			Metal				= (1 << 10),
			Nvn					= (1 << 11),
			OpenGLES			= (1 << 12),
			OpenGL				= (1 << 13),
			Vulkan				= (1 << 14),
		};

		typedef uint32_t PrimitiveUpdateFlagsType;
		typedef uint32_t WindowUpdateFlagsType;

		enum PrimitiveType
		{
			Triangles,
			Lines,
			Points
		};

		enum RenderMode
		{
			/*
			Static: Primitive is transformed and recreated rarely.
			Dynamic: Primitive is recreated rarely but is transformed frequently.
			Transient: Primitive is recreated frequently.
			*/
			Static,
			Dynamic,
			Transient
			// TODO: Split Dynamic into 'DynamicDynamic' and 'StaticDynamic'?
		};

		typedef uint32_t VertexAttributeFlagsType;
		enum VertexAttributeFlag : VertexAttributeFlagsType
		{
			Position			= (1 << 0),
			Normal				= (1 << 1),
			Tangent				= (1 << 2),
			Bitangent			= (1 << 3),
			Color0				= (1 << 4),
			Color1				= (1 << 5),
			Color2				= (1 << 6),
			Color3				= (1 << 7),
			Weight				= (1 << 9),
			TexCoord0			= (1 << 10),
			TexCoord1			= (1 << 11),
			TexCoord2			= (1 << 12),
			TexCoord3			= (1 << 13),
			TexCoord4			= (1 << 14),
			TexCoord5			= (1 << 15),
			TexCoord6			= (1 << 16),
			TexCoord7			= (1 << 17),
		};

		enum Projection
		{
			Perspective,
			Orthographic
		};

		struct ViewSize
		{
			enum ViewSizeType
			{
				Pixels,
				Relative
			};


			ViewSize() {}
			ViewSize(const float _width, const float _height, const ViewSizeType _type = Relative)
				: width(_width), height(_height), type(_type) {}


			ViewSizeType type = Relative;
			float width = 1.0f;
			float height = 1.0f;
		};

		struct AspectRatio
		{
			AspectRatio(const uint16_t _numerator, const uint16_t _denominator)
			{
				const uint16_t gcd = std::gcd(_numerator, _denominator);
				numerator = _numerator / gcd;
				denominator = _denominator / gcd;
			}
			bool operator==(const AspectRatio& _other) const
			{
				return numerator == _other.numerator && denominator == _other.denominator;
			}
			bool operator!=(const AspectRatio& _other) const
			{
				return numerator != _other.numerator || denominator != _other.denominator;
			}

			uint16_t numerator;
			uint16_t denominator;
		};

		typedef uint16_t ViewClearFlagsType;
		enum ViewClearFlag : ViewClearFlagsType
		{
			ClearColor		= (1 << 0),
			ClearDepth		= (1 << 1),
			ClearStencil	= (1 << 2),
		};
	}
}
