#pragma once

#include "SpehsEngine/Core/Color.h"
#include "bgfx/bgfx.h" // TODO: get rid of this
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include <stdint.h>

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
			MSAAX2				= (1 << 1),
			MSAAX4				= (1 << 2),
			MSAAX8				= (1 << 3),
			MSAAX16				= (1 << 4),
			VSync				= (1 << 5),
		};

		typedef uint32_t UpdateFlagsType;

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
		};

		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 uv;
			se::Color color = { 1.0f, 1.0f, 1.0f, 1.0f };

			static const bgfx::VertexLayout& getVertexLayout()
			{
				static bgfx::VertexLayout layout;
				static bool set = false;
				if (!set)
				{
					layout.begin()
						  .add(bgfx::Attrib::Position,	3, bgfx::AttribType::Float)
						  .add(bgfx::Attrib::Normal,	3, bgfx::AttribType::Float)
						  .add(bgfx::Attrib::TexCoord0,	2, bgfx::AttribType::Float)
						  .add(bgfx::Attrib::Color0,	4, bgfx::AttribType::Float, true)
						  .end();
					set = true;
				}
				return layout;
			}
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
	}
}
