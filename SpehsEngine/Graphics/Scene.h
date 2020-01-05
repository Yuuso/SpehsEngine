#pragma once

#include "SpehsEngine/Graphics/Internal/Batch.h"
#include "SpehsEngine/Graphics/Internal/PrimitiveInstance.h"
#include "SpehsEngine/Graphics/Internal/RenderContext.h"
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


			void add(const Primitive& _primitive);
			void remove(const Primitive& _primitive);

			void update();

		protected:

			friend class ViewInstance;

			void render(RenderContext& _renderContext) const;

			void batch(PrimitiveInstance& _primitive);
			void unbatch(PrimitiveInstance& _primitive);

		private:

			std::vector<std::unique_ptr<Batch>> batches;
			std::vector<std::unique_ptr<PrimitiveInstance>> primitives;
		};
	}
}
