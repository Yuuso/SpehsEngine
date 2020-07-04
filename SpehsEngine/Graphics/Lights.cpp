#include "stdafx.h"
#include "SpehsEngine/Graphics/Lights.h"

#include "SpehsEngine/Core/ColorUtilityFunctions.h"
#include "SpehsEngine/Core/Constants.h"
#include "SpehsEngine/Core/SE_Assert.h"


namespace se
{
	namespace graphics
	{
		void Light::setColor(const Color& _color)
		{
			if (color.r == _color.r &&
				color.g == _color.g &&
				color.b == _color.b)
				return;

			color.r = _color.r;
			color.g = _color.g;
			color.b = _color.b;

			dirty = true;
		}
		void Light::setIntensity(const float _intensity)
		{
			if (color.a == _intensity)
				return;

			color.a = _intensity;

			dirty = true;
		}
		void Light::setPosition(const glm::vec3& _position)
		{
			if (position == _position)
				return;

			position = _position;

			dirty = true;
		}
		void Light::setGlobal(const bool _global)
		{
			if (getGlobal() == _global)
				return;

			global = _global ? 1.0f : 0.0f;

			dirty = true;
		}
		void Light::setDirection(const glm::vec3& _direction)
		{
			if (direction == _direction)
				return;

			direction = _direction;

			dirty = true;
		}
		void Light::setRadius(const float _inner, const float _outer)
		{
			if (innerRadius == _inner && outerRadius == _outer)
				return;

			innerRadius = _inner;
			outerRadius = _outer;

			dirty = true;
		}
		void Light::setCone(const float _inner, const float _outer)
		{
			if (innerCone == _inner && outerCone == _outer)
				return;

			se_assert(_inner >= 0.0f && _inner <= TWO_PI<float>);
			se_assert(_outer >= 0.0f && _outer <= TWO_PI<float>);
			innerCone = _inner;
			outerCone = _outer;

			dirty = true;
		}

		const Color& Light::getColor() const
		{
			return color;
		}
		const float& Light::getIntensity() const
		{
			return color.a;
		}
		const glm::vec3& Light::setPosition() const
		{
			return position;
		}
		const bool Light::getGlobal() const
		{
			return global > 0.0f;
		}
		const glm::vec3& Light::setDirection() const
		{
			return direction;
		}
		const float Light::getInnerRadius() const
		{
			return innerRadius;
		}
		const float Light::getOuterRadius() const
		{
			return outerRadius;
		}
		const float Light::getInnerCone() const
		{
			return innerCone;
		}
		const float Light::getOuterCone() const
		{
			return outerCone;
		}



		AmbientLight::AmbientLight()
			: AmbientLight(Color(0.0f, 0.0f, 0.0f), 0.0f)
		{
		}
		AmbientLight::AmbientLight(const Color& _color, const float _intensity)
		{
			color = _color;
			color.a = _intensity;

			position = { 0.0f, 0.0f, 0.0f };
			global = 1.0f;
			direction = { 0.0f, 0.0f, 0.0f };
			innerRadius = FLT_MAX;
			outerRadius = FLT_MAX;
			innerCone = TWO_PI<float>;
			outerCone = TWO_PI<float>;
		}


		DirectionalLight::DirectionalLight()
			: DirectionalLight(hexColor(0xfbfbd3ff), 0.5f, { 0.1f, -1.0f, 0.1f })
		{
		}
		DirectionalLight::DirectionalLight(const Color& _color, const float _intensity, const glm::vec3& _direction)
		{
			color = _color;
			color.a = _intensity;
			direction = _direction;

			position = { 0.0f, 0.0f, 0.0f };
			global = 1.0f;
			innerRadius = FLT_MAX;
			outerRadius = FLT_MAX;
			innerCone = TWO_PI<float>;
			outerCone = TWO_PI<float>;
		}


		PointLight::PointLight()
			: PointLight(Color(), 1.0f, { 0.0f, 1.0f, 0.0f }, 0.1f, 10.0f)
		{
		}
		PointLight::PointLight(const Color& _color, const float _intensity, const glm::vec3& _position,
							   const float _innerRadius, const float _outerRadius)
		{
			color = _color;
			color.a = _intensity;
			position = _position;
			innerRadius = _innerRadius;
			outerRadius = _outerRadius;

			global = 0.0f;
			innerCone = TWO_PI<float>;
			outerCone = TWO_PI<float>;
		}


		SpotLight::SpotLight()
			: SpotLight(Color(), 1.0f, { 0.0f, 1.0f, 0.0f }, { 0.0f, -0.5f, -1.0f }, 0.1f, 10.0f, glm::radians(30.0f), glm::radians(35.0f))
		{
		}
		SpotLight::SpotLight(const Color& _color, const float _intensity, const glm::vec3& _position, const glm::vec3& _direction,
							 const float _innerRadius, const float _outerRadius, const float _innerCone, const float _outerCone)
		{
			color = _color;
			color.a = _intensity;
			position = _position;
			direction = _direction;
			innerRadius = _innerRadius;
			outerRadius = _outerRadius;
			innerCone = _innerCone;
			outerCone = _outerCone;

			global = 0.0f;
		}
	}
}
