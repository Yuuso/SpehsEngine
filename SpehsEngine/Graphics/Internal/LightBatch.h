#pragma once

#include "SpehsEngine/Graphics/Internal/LightInternal.h"
#include "SpehsEngine/Graphics/Lights.h"


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


			void preRender(const bool _forceAllUpdates);
			void bind();
			void postRender();

			void add(Light& _light);
			void remove(Light& _light);
			void clear();

		private:

			void update(const size_t _index);

			std::vector<std::unique_ptr<LightInternal>> lights;

			glm::vec4 lightInfo;
			glm::vec4 data1[SE_MAX_LIGHTS];
			glm::vec4 data2[SE_MAX_LIGHTS];
			glm::vec4 data3[SE_MAX_LIGHTS];
			glm::vec4 data4[SE_MAX_LIGHTS];

			bgfx::UniformHandle lightInfoUniform;
			bgfx::UniformHandle lightData1Uniform;
			bgfx::UniformHandle lightData2Uniform;
			bgfx::UniformHandle lightData3Uniform;
			bgfx::UniformHandle lightData4Uniform;
		};
	}
}
