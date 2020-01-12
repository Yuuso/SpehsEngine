#include "stdafx.h"
#include "SpehsEngine/Graphics/Scene.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Graphics/Shader.h"
#include "SpehsEngine/Graphics/Types.h"


namespace se
{
	namespace graphics
	{
		Scene::Scene()
		{
		}
		Scene::~Scene()
		{
		}

		void Scene::add(Primitive& _primitive)
		{
			auto it = std::find_if(primitives.begin(),
								   primitives.end(),
								   [&_primitive](const std::unique_ptr<PrimitiveInstance>& primitive)
								   {
									   return *primitive.get() == _primitive;
								   });
			if (it != primitives.end())
			{
				se::log::error("Primitive already found in scene!");
				return;
			}
			primitives.emplace_back(std::make_unique<PrimitiveInstance>(_primitive));
		}
		void Scene::remove(Primitive& _primitive)
		{
			auto it = std::find_if(primitives.begin(),
								   primitives.end(),
								   [&_primitive](const std::unique_ptr<PrimitiveInstance>& primitive)
								   {
									   return *primitive.get() == _primitive;
								   });
			if (it == primitives.end())
			{
				se::log::error("Primitive not found!");
				return;
			}

			if (it->get()->isBatched())
				it->get()->unbatch();

			it->reset(primitives.back().release());
			primitives.pop_back();
		}

		void Scene::render(RenderContext& _renderContext)
		{
			// TODO: Optimize unbatching

			for (size_t i = 0; i < primitives.size(); )
			{
				if (primitives[i]->wasDestroyed())
				{
					if (primitives[i]->isBatched())
						primitives[i]->unbatch();
					primitives[i].reset(primitives.back().release());
					primitives.pop_back();
					continue;
				}

				primitives[i]->update();
				if (primitives[i]->getRenderState())
				{
					if (primitives[i]->getRenderMode() == RenderMode::Static)
					{
						if (!primitives[i]->isBatched())
							batch(*primitives[i].get());
					}
					else
					{
						primitives[i]->render(_renderContext);
					}
				}

				i++;
			}

			for (size_t i = 0; i < batches.size(); )
			{
				if (!batches[i]->render(_renderContext))
				{
					batches[i].reset(batches.back().release());
					batches.pop_back();
					continue;
				}
				i++;
			}
		}

		void Scene::batch(PrimitiveInstance& _primitive)
		{
			se_assert(!_primitive.wasDestroyed());
			bool found = false;
			for (auto&& batch : batches)
			{
				if (batch->check(_primitive.getRenderInfo()) &&
					batch->check(_primitive.getVertices().size(), _primitive.getIndices().size()))
				{
					_primitive.batch(*batch.get());
					found = true;
					break;
				}
			}
			if (!found)
			{
				batches.emplace_back(std::make_unique<Batch>(_primitive.getRenderInfo()));
				_primitive.batch(*batches.back());
			}
		}
	}
}
