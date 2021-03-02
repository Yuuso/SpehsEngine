#pragma once

#include "SpehsEngine/Graphics/Internal/ResourceManager.h"
#include "SpehsEngine/Graphics/ModelData.h"
#include "SpehsEngine/Graphics/ResourceLoader.h"
#include "SpehsEngine/Graphics/ResourcePathFinder.h"
#include <memory>
#include <string>

namespace se
{
	namespace graphics
	{
		class ModelDataManager : public ResourceManager<ModelData>
		{
		public:

			ModelDataManager() = default;
			virtual ~ModelDataManager() = default;

			ModelDataManager(const ModelDataManager& _other) = delete;
			ModelDataManager& operator=(const ModelDataManager& _other) = delete;

			ModelDataManager(ModelDataManager&& _other) = delete;
			ModelDataManager& operator=(ModelDataManager&& _other) = delete;


			std::shared_ptr<ModelData> create(const std::string_view _name);
			std::shared_ptr<ModelData> find(const std::string_view _name) const;
		};
	}
}
