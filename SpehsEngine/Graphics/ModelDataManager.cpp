#include "stdafx.h"
#include "SpehsEngine/Graphics/ModelDataManager.h"

#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/StringViewUtilityFunctions.h"

namespace se
{
	namespace graphics
	{
		std::shared_ptr<ModelData> ModelDataManager::create(const std::string_view _name)
		{
			const std::string path = pathFinder->getPath(_name);

			for (auto& modelData : resources)
			{
				if (modelData->getName() == _name)
				{
					log::warning("Cannot create ModelData '" + _name + "', ModelData with that name already exists!");
					se_assert(modelData->path == path);
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
			se_assert_m(false, "ModelData '" + std::string(_name) + "' not found!");
			return nullptr;
		}
	}
}