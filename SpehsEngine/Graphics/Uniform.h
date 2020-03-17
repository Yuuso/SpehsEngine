#pragma once

#include "SpehsEngine/Graphics/Types.h"
#include "bgfx/bgfx.h" // !
#include "glm/vec4.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
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
*/


namespace se
{
	namespace graphics
	{
		class Uniform
		{
		public:

			~Uniform();

			Uniform(const Uniform& _other) = delete;
			Uniform& operator=(const Uniform& _other) = delete;

			Uniform(Uniform&& _other) = delete;
			Uniform& operator=(Uniform&& _other) = delete;

			bool operator==(const Uniform& _other) const;


			const UniformType getType() const;
			const uint16_t getNumElements() const;
			const std::string& getName() const;

			void set(const glm::vec4& _value, const uint16_t _numElements = 1);
			void set(const glm::mat3& _value, const uint16_t _numElements = 1);
			void set(const glm::mat4& _value, const uint16_t _numElements = 1);
			void set(const float*	  _value, const uint16_t _numElements);
			// TODO: Sampler

			Uniform(const bgfx::UniformInfo& _uniformInfo, const bgfx::UniformHandle _uniformHandle);

		private:

			friend class Shader;

			void reset(const bgfx::UniformInfo& _uniformInfo, const bgfx::UniformHandle _uniformHandle);

			std::string name;
			UniformType type;
			uint16_t numElements;

			bgfx::UniformHandle uniformHandle;
		};
	}
}
