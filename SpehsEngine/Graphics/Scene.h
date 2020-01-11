#pragma once

#include "SpehsEngine/Graphics/Internal/Batch.h"
#include "SpehsEngine/Graphics/Internal/InternalTypes.h"
#include "SpehsEngine/Graphics/Internal/PrimitiveInstance.h"
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

		protected:

			friend class ViewInstance;

			void render(RenderContext& _renderContext);

			void batch(PrimitiveInstance& _primitive);

		private:

			std::vector<std::unique_ptr<Batch>> batches;
			std::vector<std::unique_ptr<PrimitiveInstance>> primitives;
		};
	}
}
