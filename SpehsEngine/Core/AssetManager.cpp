#include "stdafx.h"
#include "SpehsEngine/Core/AssetManager.h"


namespace se
{
	bool AssetManager::isReady() const
	{
		for (auto&& asset : assets)
		{
			if (asset->isLoading())
			{
				return false;
			}
		}
		return true;
	}
	bool AssetManager::hasAsset(std::string_view _name) const
	{
		for (auto&& asset : assets)
		{
			if (asset->getName() == _name)
			{
				return true;
			}
		}
		return false;
	}
	void AssetManager::setAsyncTaskManager(std::shared_ptr<AsyncTaskManager> _taskManager)
	{
		asyncTaskManager = _taskManager;
	}
	AsyncTaskManager* AssetManager::getAsyncTaskManager()
	{
		return asyncTaskManager.get();
	}
	void AssetManager::update()
	{
		for (auto&& asset : assets)
		{
			asset->update();
		}
	}
	void AssetManager::reload()
	{
		for (auto&& asset : assets)
		{
			if (asset->isReloadable())
			{
				asset->reload(asyncTaskManager.get());
			}
		}
	}
	void AssetManager::purgeUnused()
	{
		for (size_t i = 0; i < assets.size(); /*i++*/)
		{
			if (!assets[i]->getPurgeProtection() && assets[i].use_count() == 1)
			{
				assets[i].swap(assets.back());
				assets.pop_back();
			}
			else
			{
				i++;
			}
		}
	}
	void AssetManager::purgeAll()
	{
		assets.clear();
	}
}