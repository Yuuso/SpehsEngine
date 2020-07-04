#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/LightBatch.h"


namespace se
{
	namespace graphics
	{
		LightBatch::LightBatch()
		{
			lightInfoUniform = bgfx::createUniform("u_lightInfo", bgfx::UniformType::Vec4);
			lightData1Uniform = bgfx::createUniform("u_lightData1", bgfx::UniformType::Vec4, static_cast<uint16_t>(MAX_LIGHTS));
			lightData2Uniform = bgfx::createUniform("u_lightData2", bgfx::UniformType::Vec4, static_cast<uint16_t>(MAX_LIGHTS));
			lightData3Uniform = bgfx::createUniform("u_lightData3", bgfx::UniformType::Vec4, static_cast<uint16_t>(MAX_LIGHTS));
			lightData4Uniform = bgfx::createUniform("u_lightData4", bgfx::UniformType::Vec4, static_cast<uint16_t>(MAX_LIGHTS));
		}
		LightBatch::~LightBatch()
		{
			if (bgfx::isValid(lightInfoUniform))
			{
				bgfx::destroy(lightInfoUniform);
				lightInfoUniform = BGFX_INVALID_HANDLE;
			}
			if (bgfx::isValid(lightData1Uniform))
			{
				bgfx::destroy(lightData1Uniform);
				lightData1Uniform = BGFX_INVALID_HANDLE;
			}
			if (bgfx::isValid(lightData2Uniform))
			{
				bgfx::destroy(lightData2Uniform);
				lightData2Uniform = BGFX_INVALID_HANDLE;
			}
			if (bgfx::isValid(lightData3Uniform))
			{
				bgfx::destroy(lightData3Uniform);
				lightData3Uniform = BGFX_INVALID_HANDLE;
			}
			if (bgfx::isValid(lightData4Uniform))
			{
				bgfx::destroy(lightData4Uniform);
				lightData4Uniform = BGFX_INVALID_HANDLE;
			}
		}

		void LightBatch::bind()
		{
			const size_t numLights = lights.size();

			for (size_t i = 0; i < numLights; i++)
			{
				if (lights[i].get().dirty)
				{
					update(i);
				}
			}

			const glm::vec4 lightInfo = { numLights, 0.0f, 0.0f, 0.0f };

			bgfx::setUniform(lightInfoUniform, reinterpret_cast<const void*>(&lightInfo));
			if (numLights > 0)
			{
				bgfx::setUniform(lightData1Uniform, reinterpret_cast<const void*>(&data1), static_cast<uint16_t>(numLights));
				bgfx::setUniform(lightData2Uniform, reinterpret_cast<const void*>(&data2), static_cast<uint16_t>(numLights));
				bgfx::setUniform(lightData3Uniform, reinterpret_cast<const void*>(&data3), static_cast<uint16_t>(numLights));
				bgfx::setUniform(lightData4Uniform, reinterpret_cast<const void*>(&data4), static_cast<uint16_t>(numLights));
			}
		}

		void LightBatch::add(Light& _light)
		{
			auto it = std::find_if(lights.begin(),
								   lights.end(),
								   [&_light](const std::reference_wrapper<Light>& light)
								   {
									   return &light.get() == &_light;
								   });
			if (it != lights.end())
			{
				se::log::error("Light already found in scene!");
				return;
			}

			if (lights.size() >= MAX_LIGHTS)
			{
				se::log::error("Max number of Lights reached!");
				return;
			}

			lights.push_back(_light);
			_light.dirty = true;
		}
		void LightBatch::remove(Light& _light)
		{
			auto it = std::find_if(lights.begin(),
								   lights.end(),
								   [&_light](const std::reference_wrapper<Light>& light)
								   {
									   return &light.get() == &_light;
								   });
			if (it == lights.end())
			{
				se::log::error("Light not found!");
				return;
			}

			*it = lights.back().get();
			lights.pop_back();

			it->get().dirty = true;
		}
		void LightBatch::clear()
		{
			lights.clear();
		}

		void LightBatch::update(const size_t _index)
		{
			static_assert(sizeof(Light) >= sizeof(glm::vec4) * 4, "Invalid Light size!");
			memcpy(&data1[_index], &lights[_index].get().color, sizeof(glm::vec4));
			memcpy(&data2[_index], &lights[_index].get().position, sizeof(glm::vec4));
			memcpy(&data3[_index], &lights[_index].get().direction, sizeof(glm::vec4));
			memcpy(&data4[_index], &lights[_index].get().innerRadius, sizeof(glm::vec4));
			lights[_index].get().dirty = false;
		}
	}
}
