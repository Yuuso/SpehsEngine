#pragma once

#include "SpehsEngine/Graphics/Batch.h"
#include "SpehsEngine/Graphics/Primitive.h"
#include "SpehsEngine/Graphics/PrimitiveInstance.h"
#include <vector>
#include <memory>


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

			void render() const;

		private:

			std::vector<std::unique_ptr<Batch>> batches;
			std::vector<std::unique_ptr<PrimitiveInstance>> primitives;
		};
	}
}
