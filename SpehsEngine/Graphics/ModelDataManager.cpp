#include "stdafx.h"
#include "SpehsEngine/Graphics/ModelDataManager.h"


namespace se
{
	namespace graphics
	{
		std::shared_ptr<ModelData> ModelDataManager::create(const std::string_view _name, const std::string_view _model)
		{
			const std::string path = pathFinder->getPath(_model);

			for (auto& modelData : resources)
			{
				if (modelData->getName() == _name)
				{
					log::warning("Cannot create ModelData '" + _name + "', ModelData with that name already exists!");
					if (modelData->path != path)
						log::error("Existing ModelData's '" + _name + "' resource path doesn't match!");
					return modelData;
				}
			}

			resources.push_back(std::make_shared<ModelData>(_name));
			std::shared_ptr<ModelData>& modelData = resources.back();
			modelData->create(path, resourceLoader);
			return modelData;
		}
		std::shared_ptr<ModelData> ModelDataManager::find(const std::string_view _name) const
		{
			for (auto& modelData : resources)
			{
				if (modelData->getName() == _name)
					return modelData;
			}
			return nullptr;
		}
	}
}