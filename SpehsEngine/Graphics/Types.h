#pragma once

// NOTE: No SpehsEngine includes allowed

#include "glm/vec2.hpp"
#include <stdint.h>
#include <numeric>
#include <vector>


namespace se
{
	namespace graphics
	{
		typedef uint16_t IndexType;

		typedef uint32_t RenderFlagsType;
		enum RenderFlag : RenderFlagsType
		{
			CullFrontFace			= (1 << 0),
			CullBackFace			= (1 << 1),

			DepthWrite				= (1 << 2),
			DepthTestLess			= (1 << 3),
			DepthTestGreater		= (1 << 4),

			Blending				= (1 << 5),

			BillboardSpherical		= (1 << 6),
			BillboardCylindrical	= (1 << 7),
		};
		constexpr RenderFlagsType defaultRenderFlags = RenderFlag::DepthWrite | RenderFlag::DepthTestLess;

		typedef uint32_t RendererFlagsType;
		enum RendererFlag : RendererFlagsType
		{
			CWFrontFace			= (1 << 0),
			MSAA2				= (1 << 1),
			MSAA4				= (1 << 2),
			MSAA8				= (1 << 3),
			MSAA16				= (1 << 4),
			VSync				= (1 << 5),
		};

		enum class RendererBackend
		{
			Auto,
			Direct3D9,
			Direct3D11,
			Direct3D12,
			Gnm,
			Metal,
			Nvn,
			OpenGLES,
			OpenGL,
			Vulkan,
			WebGPU
		};

		typedef uint32_t PrimitiveUpdateFlagsType;
		typedef uint32_t WindowUpdateFlagsType;

		enum PrimitiveType
		{
			Undefined,
			Triangles,
			Lines,
			Points
		};
		constexpr PrimitiveType defaultPrimitiveType = PrimitiveType::Triangles;

		enum RenderMode
		{
			/*
			Static: Primitive is transformed and recreated rarely.
			Dynamic: Primitive is recreated rarely but is transformed frequently.
			Transient: Primitive is recreated frequently.
			*/
			Static,
			Dynamic,
			//Transient
			// TODO: Split Dynamic into 'DynamicDynamic' and 'StaticDynamic'?
		};
		constexpr RenderMode defaultRenderMode = RenderMode::Dynamic;

		enum Projection
		{
			Perspective,
			Orthographic
		};

		enum class UniformType
		{
			Invalid,
			Sampler,
			Vec4,
			Mat3,
			Mat4,
		};

		enum class ViewSizeType
		{
			Pixels,
			Relative
		};

		struct ViewSize
		{
			ViewSize() = default;
			ViewSize(const float _width, const float _height, const ViewSizeType _type = ViewSizeType::Relative)
				: width(_width), height(_height), type(_type) {}

			ViewSizeType type = ViewSizeType::Relative;
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

		enum class TextureWrappingMode
		{
			Repeat,
			Mirror,
			Clamp,
			Border
		};

		enum class TextureSamplingMode
		{
			Linear,
			Point,
			Anisotrophic
		};

		struct TextureModes
		{
			TextureWrappingMode wrapU = TextureWrappingMode::Repeat;
			TextureWrappingMode wrapV = TextureWrappingMode::Repeat;
			TextureWrappingMode wrapW = TextureWrappingMode::Repeat;
			TextureSamplingMode sampleMin = TextureSamplingMode::Linear;
			TextureSamplingMode sampleMag = TextureSamplingMode::Linear;
			TextureSamplingMode sampleMip = TextureSamplingMode::Linear;
		};

		enum class FontSizeType
		{
			Pixel,
			Point
		};

		struct FontSize
		{
			FontSize()
				: size(12) {}
			FontSize(const uint32_t _size, const FontSizeType _type = FontSizeType::Pixel)
				: size(_size), type(_type) {}

			FontSizeType type;
			uint32_t size;
		};

		struct TextDimensions
		{
			glm::vec2 offsetFromOrigin; // Offset to top left from pen origin
			glm::vec2 dimensions;		// Total width and height
		};

		enum class ShapeType
		{
			Unknown,

			Triangle = 3,
			Square,
			Plane = Square,
			Rectangle = Square,
			Pentagon,
			Hexagon,
			Heptagon,
			Octagon,
			Nonegon,
			Decagon,
			Circle,

			Cube,
			Sphere,
			Ball = Sphere,
			// Capsule
			// Cylinder
			// Cone
		};

		struct ShapeParameters
		{
			bool operator==(const ShapeParameters& _other) const
			{
				return resolution == _other.resolution
					&& uvScale == _other.uvScale
					&& generateNormals == _other.generateNormals
					&& invertNormals == _other.invertNormals
					&& generateTangents == _other.generateTangents;
			}

			unsigned int resolution = 0;
			float uvScale = 1.0f;
			bool generateNormals = false;
			bool invertNormals = false;
			bool generateTangents = false;
		};

		enum class InstanceBufferType
		{
			Transform,
			Billboard
		};
	}
}
