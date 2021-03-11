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
			lightBatch = std::make_unique<LightBatch>();
		}
		Scene::~Scene()
		{
		}

		void Scene::add(Primitive& _primitive)
		{
			if (find(_primitive))
			{
				se::log::error("Primitive already found in scene!");
				return;
			}
			primitives.push_back(std::make_unique<PrimitiveInstance>(_primitive));
		}
		void Scene::remove(Primitive& _primitive)
		{
			auto it = std::find_if(primitives.begin(), primitives.end(),
								   [&_primitive](const std::unique_ptr<PrimitiveInstance>& primitive)
								   {
									   return *primitive.get() == _primitive;
								   });
			if (it == primitives.end())
			{
				se::log::error("Primitive not found in scene!");
				return;
			}

			if (it->get()->isBatched())
				it->get()->unbatch();

			it->reset(primitives.back().release());
			primitives.pop_back();
		}
		bool Scene::find(const Primitive& _primitive) const
		{
			auto it = std::find_if(primitives.begin(), primitives.end(),
								   [&_primitive](const std::unique_ptr<PrimitiveInstance>& primitive)
								   {
									   return *primitive.get() == _primitive;
								   });
			return it != primitives.end();
		}

		void Scene::add(Model& _model)
		{
			auto it = std::find_if(models.begin(), models.end(),
								   [&_model](const std::unique_ptr<ModelInstance>& model)
								   {
									   return *model.get() == _model;
								   });
			if (it != models.end())
			{
				se::log::error("Model already found in scene!");
				return;
			}
			models.push_back(std::make_unique<ModelInstance>(_model));
			// NOTE: Model primitives will be added in first preRender
		}
		void Scene::remove(Model& _model)
		{
			auto it = std::find_if(models.begin(), models.end(),
								   [&_model](const std::unique_ptr<ModelInstance>& model)
								   {
									   return *model.get() == _model;
								   });
			if (it == models.end())
			{
				se::log::error("Model not found in scene!");
				return;
			}

			it->get()->foreachPrimitive([this](Primitive& _primitive) { this->remove(_primitive); });
			it->reset(models.back().release());
			models.pop_back();
		}

		void Scene::add(Light& _light)
		{
			lightBatch->add(_light);
		}
		void Scene::remove(Light& _light)
		{
			lightBatch->remove(_light);
		}

		void Scene::clear()
		{
			models.clear();
			primitives.clear();
			batches.clear();
			lightBatch->clear();
		}

		void Scene::render(RenderContext& _renderContext)
		{
			_renderContext.lightBatch = lightBatch.get();

			// TODO: Optimize unbatching?

			for (auto&& primitive : primitives)
			{
				primitive->update();
				if (primitive->getRenderState())
				{
					if (primitive->getRenderMode() == RenderMode::Static)
					{
						if (!primitive->isBatched())
							batch(*primitive.get());
					}
					else
					{
						primitive->render(_renderContext);
					}
				}
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

		void Scene::preRender(const bool /*_renderState*/, const bool _forceAllUpdates)
		{
			if (readyToRender)
				return;
			readyToRender = true;

			for (size_t i = 0; i < models.size(); )
			{
				if (models[i]->wasDestroyed())
				{
					models[i].reset(models.back().release());
					models.pop_back();
					continue;
				}

				if (models[i]->wasReloaded())
				{
					models[i]->foreachPrimitive([this](Primitive& _primitive){ this->add(_primitive); });
				}
				else if (_forceAllUpdates)
				{
					models[i]->foreachPrimitive(
						[this](Primitive& _primitive)
						{
							if (!this->find(_primitive))
								this->add(_primitive);
						});
				}
				models[i]->preRender();
				i++;
			}

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

				primitives[i]->preRender(_forceAllUpdates);
				i++;
			}
			lightBatch->preRender(_forceAllUpdates);
		}
		void Scene::postRender(const bool _renderState)
		{
			if (!readyToRender)
				return;
			readyToRender = false;

			for (auto&& model : models)
			{
				model->postRender();
			}
			if (_renderState)
			{
				for (auto&& primitive : primitives)
				{
					primitive->postRender();
				}
			}
			lightBatch->postRender();
		}

		void Scene::batch(PrimitiveInstance& _primitive)
		{
			se_assert(!_primitive.wasDestroyed());
			bool found = false;
			for (auto&& batch : batches)
			{
				if (batch->check(_primitive.getRenderInfo()) &&
					batch->check(_primitive.getVertices()->size(), _primitive.getIndices()->size()))
				{
					_primitive.batch(*batch.get());
					found = true;
					break;
				}
			}
			if (!found)
			{
				batches.push_back(std::make_unique<Batch>(_primitive.getRenderInfo()));
				_primitive.batch(*batches.back());
			}
		}
	}
}
