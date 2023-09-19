#pragma once

#include "SpehsEngine/Graphics/ResourceManager.h"
#include "SpehsEngine/Graphics/ModelData.h"


namespace se::gfx
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


		std::shared_ptr<ModelData> create(const std::string_view _name, const std::string_view _model);
		std::shared_ptr<ModelData> find(const std::string_view _name) const;
	};
}
