#pragma once

#include "SpehsEngine/Core/Color.h"
#include "glm/vec3.hpp"


namespace se
{
	namespace graphics
	{
		constexpr size_t MAX_LIGHTS = 4;

		class Light
		{
		public:

			void				setColor(const Color& _color);
			void				setIntensity(const float _intensity);
			void				setPosition(const glm::vec3& _position);
			void				setGlobal(const bool _global);
			void				setDirection(const glm::vec3& _direction);
			void				setRadius(const float _inner, const float _outer);
			void				setCone(const float _inner, const float _outer);

			const Color&		getColor() const;
			const float&		getIntensity() const;
			const glm::vec3&	setPosition() const;
			const bool			getGlobal() const;
			const glm::vec3&	setDirection() const;
			const float			getInnerRadius() const;
			const float			getOuterRadius() const;
			const float			getInnerCone() const;
			const float			getOuterCone() const;

		protected:

			friend class LightBatch;

			// mat4[0]
			Color color;				// Color, alpha is intensity

			// mat4[1]
			glm::vec3 position;
			float global = 0.0f;		// 0.0f = light position applies, 1.0f = light applies globally

			// mat4[2]
			glm::vec3 direction;
			float __unused;

			// mat4[3]
			float innerRadius;			// Attenuation Radius
			float outerRadius;
			float innerCone;			// Spotlight cone angle (radians)
			float outerCone;

			// extra
			bool dirty = true;
		};


		class AmbientLight final : public Light
		{
		public:

			AmbientLight();
			AmbientLight(const Color& _color, const float _intensity);
		};


		class DirectionalLight final : public Light
		{
		public:

			DirectionalLight();
			DirectionalLight(const Color& _color, const float _intensity, const glm::vec3& _direction);
		};


		class PointLight final : public Light
		{
		public:

			PointLight();
			PointLight(const Color& _color, const float _intensity, const glm::vec3& _position,
					   const float _innerRadius, const float _outerRadius);
		};


		class SpotLight final : public Light
		{
		public:

			SpotLight();
			SpotLight(const Color& _color, const float _intensity, const glm::vec3& _position, const glm::vec3& _direction,
					  const float _innerRadius, const float _outerRadius, const float _innerCone, const float _outerCone);
		};
	}
}
