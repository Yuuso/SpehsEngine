#pragma once

#include "SpehsEngine/Graphics/Internal/ResourceHandle.h"
#include "SpehsEngine/Graphics/Types.h"
#include "glm/vec4.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
#include <memory>
#include <stdint.h>
#include <string>


/*
	LIST OF DEFAULT UNIFORMS

	-- bgfx ----------------
	- u_viewRect			vec4(x, y, w, h)				View rectangle for current view, in pixels
	- u_viewTexel			vec4(1/w, 1/h, ?, ?)			Inverse width and height
	- u_view				mat4							View matrix
	- u_invView				mat4							Inverted view matrix
	- u_proj				mat4							Projection matrix
	- u_invProj				mat4							Inverted projection matrix
	- u_viewProj			mat4							Concatenated view projection matrix
	- u_invViewProj			mat4							Concatenated inverted view projection matrix
	- u_model				mat4[BGFX_CONFIG_MAX_BONES]		Array of model matrices
	- u_modelView			mat4							Concatenated model[0] view matrix
	- u_modelViewProj		mat4							Concatenated model[0] view projection matrix
	- u_alphaRef			float							Alpha reference value for alpha test

	-- Spehs Engine --------
	- u_normal				mat4							Transpose of inverse of upper-left 3x3 part of model[0]
	- u_primitiveColor		vec4							Primitive color

	TODO: Update from lights and materials
*/


namespace se
{
	namespace graphics
	{
		class Font;
		class Texture;

		class Uniform
		{
		public:

			Uniform(const std::string_view _name, const UniformType _type, const uint16_t _numElements = 1);
			~Uniform();

			Uniform(const Uniform& _other) = delete;
			Uniform& operator=(const Uniform& _other) = delete;

			Uniform(Uniform&& _other) = delete;
			Uniform& operator=(Uniform&& _other) = delete;

			bool operator==(const Uniform& _other) const;


			const std::string& getName() const;
			const UniformType getType() const;
			const uint16_t getNumElements() const;

			void set(const glm::vec4* _value, const uint16_t _numElements = 1);
			void set(const glm::mat3* _value, const uint16_t _numElements = 1);
			void set(const glm::mat4* _value, const uint16_t _numElements = 1);
			void set(const Texture& _value, const uint8_t _stage);
			void set(const Font& _value, const uint8_t _stage);

		private:

			const std::string name;
			const UniformType type;
			const uint16_t numElements;

			ResourceHandle uniformHandle = INVALID_RESOURCE_HANDLE;
		};
	}
}
