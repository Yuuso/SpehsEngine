#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/DefaultUniforms.h"

#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Graphics/Renderer.h"

#include <chrono>


namespace se
{
	namespace graphics
	{
		DefaultUniforms::DefaultUniforms()
			: normal(BGFX_INVALID_HANDLE)
			, ambientLight_ColorIntensity(BGFX_INVALID_HANDLE)
			, pointLight_Count(BGFX_INVALID_HANDLE)
			, pointLight_PositionIRadius(BGFX_INVALID_HANDLE)
			, pointLight_ColorORadius(BGFX_INVALID_HANDLE)
		{
			reload();
		}
		DefaultUniforms::~DefaultUniforms()
		{
			destroy();
		}

		void DefaultUniforms::destroy()
		{
			if (bgfx::isValid(normal))
			{
				bgfx::destroy(normal);
				normal = BGFX_INVALID_HANDLE;
			}
			if (bgfx::isValid(ambientLight_ColorIntensity))
			{
				bgfx::destroy(ambientLight_ColorIntensity);
				ambientLight_ColorIntensity = BGFX_INVALID_HANDLE;
			}
			if (bgfx::isValid(pointLight_Count))
			{
				bgfx::destroy(pointLight_Count);
				pointLight_Count = BGFX_INVALID_HANDLE;
			}
			if (bgfx::isValid(pointLight_PositionIRadius))
			{
				bgfx::destroy(pointLight_PositionIRadius);
				pointLight_PositionIRadius = BGFX_INVALID_HANDLE;
			}
			if (bgfx::isValid(pointLight_ColorORadius))
			{
				bgfx::destroy(pointLight_ColorORadius);
				pointLight_ColorORadius = BGFX_INVALID_HANDLE;
			}
		}
		void DefaultUniforms::reload()
		{
			destroy();
			normal = bgfx::createUniform("u_normal", bgfx::UniformType::Mat4);
			ambientLight_ColorIntensity = bgfx::createUniform("u_ambientLight_ColorIntensity", bgfx::UniformType::Vec4);
			pointLight_Count = bgfx::createUniform("u_pointLight_Count", bgfx::UniformType::Vec4);
			pointLight_PositionIRadius = bgfx::createUniform("u_pointLight_PositionIRadius", bgfx::UniformType::Vec4, MAX_NUM_LIGHTS);
			pointLight_ColorORadius = bgfx::createUniform("u_pointLight_ColorORadius", bgfx::UniformType::Vec4, MAX_NUM_LIGHTS);
		}

		void DefaultUniforms::setNormalMatrix(const glm::mat4& _normal)
		{
			bgfx::setUniform(normal, reinterpret_cast<const void*>(&_normal));
		}

		void DefaultUniforms::setLightParams()
		{
			constexpr size_t numLights = 2;
			const glm::vec4 numLightsData = { (float)numLights, 0.0f, 0.0f, 0.0f };
			const struct { glm::vec3 color = { 0.03f, 0.03f, 0.03f }; float intensity = 1.0f; } ambient;
			const double seconds = time::now().asSeconds<double>();
			const struct { glm::vec3 pos; float innerRadius; } lightPosIRad[numLights] = {
				{ { 0.0f,								5.0f,	0.0f								}, 0.0f },
				{ { (float)cos(seconds * 0.5) * 12.0f,	0.0f,	(float)sin(seconds * 0.5) * 12.0f	}, 10.0f },
			};
			const struct { glm::vec3 color; float innerRadius; } lightColorORad[numLights] = {
				{ { 1.0f, 1.0f, 1.0f }, 50.0f },
				{ { 1.0f, 1.0f, 1.0f }, 11.0f },
			};

			bgfx::setUniform(ambientLight_ColorIntensity, reinterpret_cast<const void*>(&ambient));
			bgfx::setUniform(pointLight_Count, reinterpret_cast<const void*>(&numLightsData));
			bgfx::setUniform(pointLight_PositionIRadius, reinterpret_cast<const void*>(&lightPosIRad), numLights);
			bgfx::setUniform(pointLight_ColorORadius, reinterpret_cast<const void*>(&lightColorORad), numLights);
		}
	}
}
