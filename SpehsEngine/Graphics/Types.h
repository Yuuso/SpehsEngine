#pragma once

// NOTE: No SpehsEngine includes allowed


namespace se
{
	namespace graphics
	{
		typedef uint16_t IndexType;

		typedef uint32_t RenderFlagsType;
		enum RenderFlag : RenderFlagsType
		{
			CullFrontFace				= (1 << 0),
			CullBackFace				= (1 << 1),

			WriteDepth					= (1 << 2),
			DepthTestAlways				= (1 << 3),
			DepthTestNever				= (1 << 4),
			DepthTestEqual				= (1 << 5),
			DepthTestNotEqual			= (1 << 6),
			DepthTestLess				= (1 << 7),
			DepthTestLessOrEqual		= (1 << 8),
			DepthTestGreater			= (1 << 9),
			DepthTestGreaterOrEqual		= (1 << 10),

			WriteAlpha					= (1 << 11),
			BlendAlpha					= (1 << 12),
			BlendAdd					= (1 << 13),
			BlendDarken					= (1 << 14),
			BlendLighten				= (1 << 15),
			BlendMultiply				= (1 << 16),
			BlendNormal					= (1 << 17),
			BlendScreen					= (1 << 18),
			BlendBurn					= (1 << 19),

			BillboardSpherical			= (1 << 20),
			BillboardCylindrical		= (1 << 21),
		};
		constexpr RenderFlagsType defaultRenderFlags = RenderFlag::WriteDepth | RenderFlag::DepthTestLess;

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
			Noop,

			Agc,
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

		struct DisplayMode
		{
			int width;
			int height;
			int refreshRate;
		};

		typedef uint32_t PrimitiveUpdateFlagsType;
		typedef uint32_t WindowUpdateFlagsType;
		typedef uint32_t PrimitiveAttributeFlagsType;

		enum class PrimitiveType
		{
			Undefined,
			Triangles,
			Lines,
			Points
		};
		constexpr PrimitiveType defaultPrimitiveType = PrimitiveType::Triangles;

		enum class RenderMode
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

		enum class Projection
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

		enum class DrawCallSortOrder
		{
			Default,
			Sequential,
			DepthAscending,
			DepthDescending
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
				: type(FontSizeType::Pixel), size(12) {}
			FontSize(const uint32_t _size, const FontSizeType _type = FontSizeType::Pixel)
				: type(_type), size(_size) {}

			FontSizeType type;
			uint32_t size;
		};

		struct TextDimensions
		{
			glm::vec2 offsetFromOrigin;		// Offset to top left from pen origin
			glm::vec2 dimensions;			// Total width and total height
			glm::vec2 capsDimensions;		// Total width and cap height
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
			Box = Cube,
			Sphere,
			Ball = Sphere,
			// Capsule
			// Cylinder
			// Cone
		};

		// Coordinate system orientation for 2D shapes
		enum class ShapeOrientation
		{
			XZ_Plane,
			XY_Plane,
		};

		struct ShapeParameters
		{
			bool operator==(const ShapeParameters& _other) const
			{
				return orientation == _other.orientation
					&& resolution == _other.resolution
					&& uvScale == _other.uvScale
					&& generateNormals == _other.generateNormals
					&& invertNormals == _other.invertNormals
					&& generateTangents == _other.generateTangents;
			}

			ShapeOrientation orientation = ShapeOrientation::XZ_Plane;
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

		typedef std::vector<glm::mat4> UniformMatrices;

		struct Scissor
		{
			Scissor() = default;
			Scissor(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height)
				: x(_x), y(_y), width(_width), height(_height), enabled(true) {}

			bool operator==(const Scissor& _other) const
			{
				if (!enabled && !_other.enabled)
					return true;
				return enabled	== _other.enabled
					&& x		== _other.x
					&& y		== _other.y
					&& width	== _other.width
					&& height	== _other.height;
			}

			bool enabled = false;
			// (0, 0) is top left
			uint16_t x = 0;
			uint16_t y = 0;
			uint16_t width = 0;
			uint16_t height = 0;
		};

		enum class StencilTest
		{
			Less,
			LessOrEqual,
			Equal,
			GreaterOrEqual,
			Greater,
			NotEqual,
			Never,
			Always
		};
		enum class StencilOperation
		{
			Zero,
			Keep,
			Replace,
			Increment,
			IncrementSaturate,
			Decrement,
			DecrementSaturate,
			Invert
		};
		struct Stencil
		{
			uint32_t refValue = 0x00;
			uint32_t maskValue = 0xff;
			StencilTest test = StencilTest::Always;
			StencilOperation stencilFail = StencilOperation::Keep;	// Stencil test failed, fragment discarded
			StencilOperation depthFail = StencilOperation::Keep;	// Stencil test passed but depth test failed, fragment discarded
			StencilOperation pass = StencilOperation::Keep;			// Both tests passed
		};

		enum class RenderSortDepth
		{
			Default,
			ZPosition,
		};

		enum class TextureFormat : uint8_t
		{
			Unknown,
			RGBA8,
			R8,
		};

		struct TextureInput
		{
			typedef TextureFormat Format;

			bool isCubemap = false; // +x, -x, +y, -y, +z, -z
			bool isMutable = false; // Allows use of Texture::setTextureData()
			TextureFormat format = TextureFormat::RGBA8;
			uint16_t width = 0;
			uint16_t height = 0;
			uint16_t layerCount = 1;
			std::vector<uint8_t> data;
		};
	}
}
