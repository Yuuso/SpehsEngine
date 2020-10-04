#pragma once

#include "SpehsEngine/Graphics/Internal/Batch.h"
#include "SpehsEngine/Graphics/Internal/InternalTypes.h"
#include "SpehsEngine/Graphics/Internal/LightBatch.h"
#include "SpehsEngine/Graphics/Internal/PrimitiveInstance.h"
#include "SpehsEngine/Graphics/Lights.h"
#include "SpehsEngine/Graphics/Primitive.h"
#include <memory>
#include <vector>


namespace se
{
	namespace graphics
	{
		class Scene
		{
		public:

			Scene();
			~Scene();

			Scene(const Scene& _other) = delete;
			Scene& operator=(const Scene& _other) = delete;

			Scene(Scene&& _other) = delete;
			Scene& operator=(Scene&& _other) = delete;


			void add(Primitive& _primitive);
			void remove(Primitive& _primitive);

			void add(Light& _light);
			void remove(Light& _light);

			void clearPrimitives();
			void clearLights();
			void clear();

		private:

			friend class ViewInstance;

			void render(RenderContext& _renderContext);
			void postRender();
			void batch(PrimitiveInstance& _primitive);

			std::vector<std::unique_ptr<Batch>> batches;
			std::vector<std::unique_ptr<PrimitiveInstance>> primitives;
			std::unique_ptr<LightBatch> lightBatch;
		};
	}
}
