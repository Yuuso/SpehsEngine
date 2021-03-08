#include "stdafx.h"
#include "SpehsEngine/Graphics/Lights.h"

#include "SpehsEngine/Core/ColorUtilityFunctions.h"
#include "SpehsEngine/Core/Constants.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Math/Mathematics.h"


namespace se
{
	namespace graphics
	{
		Light::~Light()
		{
			destroyedSignal();
		}
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

			if (debugPrimitive)
				debugPrimitive->setRenderState(getIntensity() > 0.0f);
		}
		void Light::setPosition(const glm::vec3& _position)
		{
			if (position == _position)
				return;

			position = _position;
			dirty = true;

			if (debugPrimitive)
				debugPrimitive->setPosition(getPosition());
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

			direction = glm::normalize(_direction);
			dirty = true;

			if (debugPrimitive)
			{
				constexpr glm::vec3 upVec = glm::vec3(0.0f, 1.0f, 0.0f);
				constexpr glm::vec3 altVec = glm::vec3(0.0f, 0.0f, 1.0f);
				debugPrimitive->setRotation(glm::quatLookAt(getDirection(), getDirection() == upVec ? altVec : upVec));
			}
		}
		void Light::setRadius(const float _inner, const float _outer)
		{
			if (innerRadius == _inner && outerRadius == _outer)
				return;

			innerRadius = _inner;
			outerRadius = _outer;
			dirty = true;

			if (debugPrimitive)
				debugPrimitive->setScale(glm::vec3(getOuterRadius()));
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

			if (debugPrimitive)
				generateDebugPrimitive();
		}

		const Color& Light::getColor() const
		{
			return color;
		}
		const float& Light::getIntensity() const
		{
			return color.a;
		}
		const glm::vec3& Light::getPosition() const
		{
			return position;
		}
		const bool Light::getGlobal() const
		{
			return global > 0.0f;
		}
		const glm::vec3& Light::getDirection() const
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

		std::shared_ptr<Primitive> Light::getDebugPrimitive()
		{
			if (!debugPrimitive)
				generateDebugPrimitive();
			return debugPrimitive;
		}



		AmbientLight::AmbientLight()
			: AmbientLight(Color(0.0f, 0.0f, 0.0f), 0.0f)
		{
		}
		AmbientLight::AmbientLight(const Color& _color, const float _intensity)
			: Light()
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
			: Light()
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
			: Light()
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
		void PointLight::generateDebugPrimitive()
		{
			if (!debugPrimitive)
			{
				debugPrimitive = std::make_shared<Primitive>();
				debugPrimitive->setPrimitiveType(PrimitiveType::Lines);
				debugPrimitive->enableRenderFlag(RenderFlag::Blending);
				debugPrimitive->setPosition(getPosition());
				debugPrimitive->setScale(glm::vec3(getOuterRadius()));
				debugPrimitive->setRenderState(getIntensity() > 0.0f);
				debugPrimitive->setColor(Color(1.0f, 1.0f, 1.0f, 0.75f));

				RenderCopy copy;
				copy.primitiveColor = Color(1.0f, 1.0f, 1.0f, 0.15f);
				copy.primitiveType = PrimitiveType::Lines;
				enableBit(copy.renderFlags, RenderFlag::Blending | RenderFlag::DepthTestGreater);
				debugPrimitive->setRenderCopy(copy);
			}

			std::shared_ptr<VertexBuffer> newVertexBuffer = std::make_shared<VertexBuffer>();
			VertexBuffer& newVertices = *newVertexBuffer.get();
			std::shared_ptr<IndexBuffer> newIndexBuffer = std::make_shared<IndexBuffer>();
			IndexBuffer& newIndices = *newIndexBuffer.get();
			using namespace VertexAttribute;
			newVertices.setAttributes(Position);

			const size_t resolution = 64;
			newVertices.resize(resolution * 3);
			newIndices.resize(resolution * 6);

			size_t currentIndex = 0;
			size_t currentVertex = 0;

			for (size_t i = 0; i < 2; i++)
			{
				const float lat = static_cast<float>(i) * HALF_PI<float>;
				for (size_t j = 0; j < resolution; j++)
				{
					const float lon = se::lerp(0.0f, TWO_PI<float>, static_cast<float>(j) / static_cast<float>(resolution));
					newVertices.get<Position>(currentVertex) = glm::vec3(sinf(lon) * cosf(lat), cosf(lon), sinf(lon) * sinf(lat));

					if (j > 0)
					{
						newIndices[currentIndex++] = static_cast<IndexType>(currentVertex - 1);
						newIndices[currentIndex++] = static_cast<IndexType>(currentVertex);
					}

					currentVertex++;
				}

				newIndices[currentIndex++] = static_cast<IndexType>(currentVertex - 1);
				newIndices[currentIndex++] = static_cast<IndexType>(i * resolution);
			}
			{
				const float lon = HALF_PI<float>;
				for (size_t j = 0; j < resolution; j++)
				{
					const float lat = se::lerp(0.0f, TWO_PI<float>, static_cast<float>(j) / static_cast<float>(resolution));
					newVertices.get<Position>(currentVertex) = glm::vec3(sinf(lon) * cosf(lat), cosf(lon), sinf(lon) * sinf(lat));

					if (j > 0)
					{
						newIndices[currentIndex++] = static_cast<IndexType>(currentVertex - 1);
						newIndices[currentIndex++] = static_cast<IndexType>(currentVertex);
					}

					currentVertex++;
				}
				newIndices[currentIndex++] = static_cast<IndexType>(currentVertex - 1);
				newIndices[currentIndex++] = static_cast<IndexType>(2 * resolution);
			}
			debugPrimitive->setVertices(newVertexBuffer);
			debugPrimitive->setIndices(newIndexBuffer);
		}


		SpotLight::SpotLight()
			: SpotLight(Color(), 1.0f, { 0.0f, 1.0f, 0.0f }, { 0.0f, -0.5f, -1.0f }, 0.1f, 10.0f, glm::radians(30.0f), glm::radians(35.0f))
		{
		}
		SpotLight::SpotLight(const Color& _color, const float _intensity, const glm::vec3& _position, const glm::vec3& _direction,
							 const float _innerRadius, const float _outerRadius, const float _innerCone, const float _outerCone)
			: Light()
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
		void SpotLight::generateDebugPrimitive()
		{
			if (!debugPrimitive)
			{
				debugPrimitive = std::make_shared<Primitive>();
				debugPrimitive->setPrimitiveType(PrimitiveType::Lines);
				debugPrimitive->enableRenderFlag(RenderFlag::Blending);
				debugPrimitive->setPosition(getPosition());
				debugPrimitive->setScale(glm::vec3(getOuterRadius()));
				debugPrimitive->setRotation(glm::quatLookAt(getDirection(), glm::vec3(0.0f, 1.0f, 0.0f)));
				debugPrimitive->setRenderState(getIntensity() > 0.0f);
				debugPrimitive->setColor(Color(1.0f, 1.0f, 1.0f, 0.75f));

				RenderCopy copy;
				copy.primitiveColor = Color(1.0f, 1.0f, 1.0f, 0.15f);
				copy.primitiveType = PrimitiveType::Lines;
				enableBit(copy.renderFlags, RenderFlag::Blending | RenderFlag::DepthTestGreater);
				debugPrimitive->setRenderCopy(copy);
			}

			if (getOuterCone() <= 0.0f)
			{
				debugPrimitive->setVertices(nullptr);
				debugPrimitive->setIndices(nullptr);
				return;
			}

			std::shared_ptr<VertexBuffer> newVertexBuffer = std::make_shared<VertexBuffer>();
			VertexBuffer& newVertices = *newVertexBuffer.get();
			std::shared_ptr<IndexBuffer> newIndexBuffer = std::make_shared<IndexBuffer>();
			IndexBuffer& newIndices = *newIndexBuffer.get();
			using namespace VertexAttribute;
			newVertices.setAttributes(Position);

			const size_t resolution = se::lerp(8, 64, getOuterCone() / TWO_PI<float>);
			newVertices.resize(resolution * 2 + 1);
			newIndices.resize(resolution * 4 + 4);

			size_t currentIndex = 0;
			size_t currentVertex = 0;

			newVertices.get<Position>(currentVertex) = glm::vec3(0.0f);
			currentVertex++;

			for (size_t i = 0; i < 2; i++)
			{
				const float lat = static_cast<float>(i) * HALF_PI<float>;
				for (size_t j = 0; j < resolution; j++)
				{
					const float lon = se::lerp(-getOuterCone() * 0.5f, getOuterCone() * 0.5f, static_cast<float>(j) / static_cast<float>(resolution - 1));
					newVertices.get<Position>(currentVertex) = glm::normalize(glm::vec3(sinf(lon) * cosf(lat), sinf(lon) * sinf(lat), -cosf(lon)));

					if (j == 0)
					{
						newIndices[currentIndex++] = static_cast<IndexType>(0);
						newIndices[currentIndex++] = static_cast<IndexType>(currentVertex);
					}
					else
					{
						newIndices[currentIndex++] = static_cast<IndexType>(currentVertex - 1);
						newIndices[currentIndex++] = static_cast<IndexType>(currentVertex);
					}

					currentVertex++;
				}

				newIndices[currentIndex++] = static_cast<IndexType>(currentVertex - 1);
				newIndices[currentIndex++] = static_cast<IndexType>(0);
			}
			se_assert(currentVertex == newVertices.size());
			se_assert(currentIndex == newIndices.size());
			debugPrimitive->setVertices(newVertexBuffer);
			debugPrimitive->setIndices(newIndexBuffer);
		}
	}
}
