#include "stdafx.h"
#include "SpehsEngine/Graphics/Scene.h"

#include "SpehsEngine/Graphics/Shader.h"


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


			bool found = false;
			for (auto&& batch : batches)
			{
				if (batch->check(primitives.back()->primitive.renderFlags) ||
					batch->check(primitives.back()->primitive.vertices.size(), primitives.back()->primitive.indices.size()))
				{
					primitives.back()->batchPosition = batch->add(primitives.back()->primitive.vertices, primitives.back()->primitive.indices);
					primitives.back()->batch = batch.get();
					found = true;
				}
			}
			if (!found)
			{
				batches.emplace_back(std::make_unique<Batch>(primitives.back()->primitive.renderFlags, primitives.back()->primitive.shader->programHandle));
				primitives.back()->batchPosition = batches.back()->add(primitives.back()->primitive.vertices, primitives.back()->primitive.indices);
				primitives.back()->batch = batches.back().get();
			}
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
				it->get()->batch->remove(it->get()->batchPosition);
			}

			primitives.erase(it);
		}

		void Scene::update()
		{
			//
		}

		void Scene::render() const
		{
			for (auto&& batch : batches)
				batch->render();
		}
	}
}
