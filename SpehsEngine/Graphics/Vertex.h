#pragma once

#include "SpehsEngine/Core/Color.h"
#include "bgfx/bgfx.h" // TODO: No bgfx includes in headers!
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"


namespace se
{
	namespace graphics
	{

		// TODO...

		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 uv;
			se::Color color = {1.0f, 1.0f, 1.0f, 0.5f};

			static const bgfx::VertexLayout& getVertexLayout()
			{
				static bgfx::VertexLayout layout;
				static bool set = false;
				if (!set)
				{
					layout.begin()
						.add(bgfx::Attrib::Position,	3, bgfx::AttribType::Float)
						.add(bgfx::Attrib::Normal,		3, bgfx::AttribType::Float)
						.add(bgfx::Attrib::TexCoord0,	2, bgfx::AttribType::Float)
						.add(bgfx::Attrib::Color0,		4, bgfx::AttribType::Float, true)
						.end();
					set = true;
				}
				return layout;
			}
		};
	}
}
