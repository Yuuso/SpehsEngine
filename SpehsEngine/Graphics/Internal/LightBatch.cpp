#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/LightBatch.h"


namespace se
{
	namespace graphics
	{
		LightBatch::LightBatch()
		{
			lightInfoUniform = bgfx::createUniform("u_lightInfo", bgfx::UniformType::Vec4);
			lightData1Uniform = bgfx::createUniform("u_lightData1", bgfx::UniformType::Vec4, static_cast<uint16_t>(SE_MAX_LIGHTS));
			lightData2Uniform = bgfx::createUniform("u_lightData2", bgfx::UniformType::Vec4, static_cast<uint16_t>(SE_MAX_LIGHTS));
			lightData3Uniform = bgfx::createUniform("u_lightData3", bgfx::UniformType::Vec4, static_cast<uint16_t>(SE_MAX_LIGHTS));
			lightData4Uniform = bgfx::createUniform("u_lightData4", bgfx::UniformType::Vec4, static_cast<uint16_t>(SE_MAX_LIGHTS));
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

		void LightBatch::preRender(const bool _forceAllUpdates)
		{
			for (size_t i = 0; i < lights.size(); )
			{
				if (lights[i]->wasDestroyed())
				{
					lights[i].reset(lights.back().release());
					lights.pop_back();
					continue;
				}

				if (lights[i]->getLight().dirty || _forceAllUpdates)
				{
					update(i);
				}
				i++;
			}
			lightInfo = { lights.size(), 0.0f, 0.0f, 0.0f };
		}

		void LightBatch::bind()
		{
			const size_t numLights = lights.size();
			bgfx::setUniform(lightInfoUniform, reinterpret_cast<const void*>(&lightInfo));
			if (numLights > 0)
			{
				bgfx::setUniform(lightData1Uniform, reinterpret_cast<const void*>(&data1), static_cast<uint16_t>(numLights));
				bgfx::setUniform(lightData2Uniform, reinterpret_cast<const void*>(&data2), static_cast<uint16_t>(numLights));
				bgfx::setUniform(lightData3Uniform, reinterpret_cast<const void*>(&data3), static_cast<uint16_t>(numLights));
				bgfx::setUniform(lightData4Uniform, reinterpret_cast<const void*>(&data4), static_cast<uint16_t>(numLights));
			}
		}

		void LightBatch::postRender()
		{
			// Clear flags after all scenes have had the chance to update
			for (auto&& light : lights)
			{
				light->getLight().dirty = false;
			}
		}

		void LightBatch::add(Light& _light)
		{
			auto it = std::find_if(lights.begin(),
								   lights.end(),
								   [&_light](const std::unique_ptr<LightInternal>& light)
								   {
									   return *light.get() == _light;
								   });
			if (it != lights.end())
			{
				se::log::error("Light already found in scene!");
				return;
			}

			if (lights.size() >= SE_MAX_LIGHTS)
			{
				se::log::error("Max number of Lights reached!");
				return;
			}

			lights.push_back(std::make_unique<LightInternal>(_light));

			// Force update buffer
			_light.dirty = true;
		}

		void LightBatch::remove(Light& _light)
		{
			auto it = std::find_if(lights.begin(),
								   lights.end(),
								   [&_light](const std::unique_ptr<LightInternal>& light)
								   {
									   return *light.get() == _light;
								   });
			if (it == lights.end())
			{
				se::log::error("Light not found!");
				return;
			}

			it->reset(lights.back().release());
			lights.pop_back();

			// Location in buffer changed
			it->get()->getLight().dirty = true;
		}

		void LightBatch::clear()
		{
			lights.clear();
		}

		void LightBatch::update(const size_t _index)
		{
			static_assert(sizeof(Light) >= sizeof(glm::vec4) * 4, "Invalid Light size!");
			static_assert(sizeof(Color) == sizeof(glm::vec4));
			memcpy(&data1[_index], &lights[_index]->getLight().color, sizeof(glm::vec4));
			memcpy(&data2[_index], &lights[_index]->getLight().position, sizeof(glm::vec4));
			memcpy(&data3[_index], &lights[_index]->getLight().direction, sizeof(glm::vec4));
			memcpy(&data4[_index], &lights[_index]->getLight().innerRadius, sizeof(glm::vec4));
		}
	}
}
