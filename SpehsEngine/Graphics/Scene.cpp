#include "stdafx.h"
#include "SpehsEngine/Graphics/Scene.h"

#include "SpehsEngine/Graphics/Shader.h"
#include "SpehsEngine/Graphics/Internal/BatchPosition.h"
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

			if (it->get()->batch)
				unbatch(*it->get());

			primitives.erase(it);
		}

		void Scene::render(RenderContext& _renderContext)
		{
			// TODO: Organize!!
			for (auto it = primitives.begin(); it != primitives.end(); )
			{
				// TODO: ?
				if (!it->get()->primitive)
				{
					it = primitives.erase(it);
					continue;
				}

				// TODO: Check update
				if (it->get()->primitive->getRenderState())
				{
					if (it->get()->primitive->getRenderMode() == RenderMode::Static)
					{
						if (it->get()->batch == nullptr)
						{
							batch(*it->get());
						}
					}
					else
					{
						// TODO: A bunch of stuff, including Transient rendering
						it->get()->updateTransformMatrix();
					}
				}
				else
				{
					if (!it->get()->batch)
						unbatch(*it->get());
					it->get()->destroyBuffers();
				}

				it++;
			}

			for (auto&& primitive : primitives)
			{
				if (primitive->primitive != nullptr
					&& primitive->primitive->getRenderState()
					&& primitive->batch == nullptr)
				{
					primitive->render(_renderContext);
				}
			}

			for (auto&& batch : batches)
				batch->render(_renderContext);
		}

		void Scene::batch(PrimitiveInstance& _primitive)
		{
			se_assert(!_primitive.batch);
			bool found = false;
			for (auto&& batch : batches)
			{
				if (batch->check(_primitive.primitive->getRenderFlags()) ||
					batch->check(_primitive.primitive->getVertices().size(), _primitive.primitive->getIndices().size()))
				{
					_primitive.batchPosition = batch->add(_primitive.primitive->getVertices(), _primitive.primitive->getIndices());
					_primitive.batch = batch.get();
					found = true;
				}
			}
			if (!found)
			{
				se_assert(_primitive.primitive->getShader() != nullptr);
				batches.emplace_back(std::make_unique<Batch>(_primitive.primitive->getRenderFlags(), *_primitive.primitive->getShader()));
				_primitive.batchPosition = batches.back()->add(_primitive.primitive->getVertices(), _primitive.primitive->getIndices());
				_primitive.batch = batches.back().get();
			}
		}
		void Scene::unbatch(PrimitiveInstance& _primitive)
		{
			se_assert(_primitive.batch);
			_primitive.batch->remove(_primitive.batchPosition);
			_primitive.batch = nullptr;
		}
	}
}
