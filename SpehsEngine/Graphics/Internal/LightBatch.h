#pragma once

#include "bgfx/bgfx.h" // !
#include "glm/vec4.hpp"
#include "SpehsEngine/Graphics/Lights.h"
#include <functional>
#include <vector>


namespace se
{
	namespace graphics
	{
		class LightBatch
		{
		public:

			LightBatch();
			~LightBatch();

			LightBatch(const LightBatch& _other) = delete;
			LightBatch& operator=(const LightBatch& _other) = delete;

			LightBatch(LightBatch&& _other) = delete;
			LightBatch& operator=(LightBatch&& _other) = delete;


			void update();
			void bind();
			void postRender();

			void add(Light& _light);
			void remove(Light& _light);
			void clear();

		private:

			void update(const size_t _index);

			std::vector<std::reference_wrapper<Light>> lights;

			glm::vec4 lightInfo;
			glm::vec4 data1[MAX_LIGHTS];
			glm::vec4 data2[MAX_LIGHTS];
			glm::vec4 data3[MAX_LIGHTS];
			glm::vec4 data4[MAX_LIGHTS];

			bgfx::UniformHandle lightInfoUniform;
			bgfx::UniformHandle lightData1Uniform;
			bgfx::UniformHandle lightData2Uniform;
			bgfx::UniformHandle lightData3Uniform;
			bgfx::UniformHandle lightData4Uniform;
		};
	}
}
