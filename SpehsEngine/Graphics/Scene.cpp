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

		void Scene::add(const Primitive& _primitive)
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
		void Scene::remove(const Primitive& _primitive)
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
			{
				unbatch(*it->get());
			}

			primitives.erase(it);
		}

		void Scene::update()
		{
			for (auto&& primitive : primitives)
			{
				// TODO: Check update
				if (primitive->primitive.getRenderState())
				{
					if (primitive->primitive.getRenderMode() == RenderMode::Static)
					{
						if (primitive->batch == nullptr)
						{
							batch(*primitive.get());
						}
					}
					else
					{
						// TODO: A bunch of stuff, including Transient rendering
						primitive->updateTransformMatrix();
					}
				}
				else
				{
					if (primitive->batch != nullptr)
						unbatch(*primitive.get());
					primitive->destroyBuffers();
				}
			}
		}

		void Scene::render(RenderContext& _renderContext) const
		{
			for (auto&& primitive : primitives)
			{
				if (primitive->primitive.getRenderState()
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
				if (batch->check(_primitive.primitive.getRenderFlags()) ||
					batch->check(_primitive.primitive.getVertices().size(), _primitive.primitive.getIndices().size()))
				{
					_primitive.batchPosition = batch->add(_primitive.primitive.getVertices(), _primitive.primitive.getIndices());
					_primitive.batch = batch.get();
					found = true;
				}
			}
			if (!found)
			{
				se_assert(_primitive.primitive.getShader() != nullptr);
				batches.emplace_back(std::make_unique<Batch>(_primitive.primitive.getRenderFlags(), *_primitive.primitive.getShader()));
				_primitive.batchPosition = batches.back()->add(_primitive.primitive.getVertices(), _primitive.primitive.getIndices());
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
