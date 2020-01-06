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
			None				= 0,
			FrontFaceCulling	= (1 << 0),
			BackFaceCulling		= (1 << 1),
			Blending			= (1 << 2),
			DepthTest			= (1 << 3),
		};

		typedef uint32_t UpdateFlagsType;
		enum UpdateFlag : UpdateFlagsType
		{
			NothingChanged		= 0,
			TransformChanged	= (1 << 0),
			RenderInfoChanged	= (1 << 1),
			VerticesChanged		= (1 << 2),
			IndicesChanged		= (1 << 3),
		};

		enum RenderStyle
		{
			Triangles,
			TriangleStrip,
			Lines,
			LineStrip,
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
	}
}
