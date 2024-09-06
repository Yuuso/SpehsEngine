#pragma once

#include "SpehsEngine/Core/AsyncTaskManager.h"
#include "SpehsEngine/Core/Asset.h"
#include <typeinfo>
#include <typeindex>


namespace se
{
	class AssetManager
	{
	public:

		SE_NO_COPY_OR_MOVE(AssetManager);
		AssetManager() = default;
		~AssetManager() = default;

		// Returns true if all assets are ready
		bool isReady() const;

		// Returns true if an asset with name is found
		bool hasAsset(std::string_view _name) const;

		// AsyncTaskManager is passed to assets in emplace function
		void setAsyncTaskManager(std::shared_ptr<AsyncTaskManager> _taskManager);
		AsyncTaskManager* getAsyncTaskManager();

		// Should be called every frame to update async task status
		void update();

		// Reload all assets that are reloadable
		void reload();

		// Remove all unprotected assets that are not referenced outside of this manager
		void purgeUnused();

		// Remove all assets
		void purgeAll();

		// Return asset with name, nullptr if not found
		template<typename T>
		requires std::derived_from<T, IAsset>
		std::shared_ptr<T> find(std::string_view _name) const
		{
			for (auto&& asset : assets)
			{
				if (asset->getName() == _name)
				{
					return std::dynamic_pointer_cast<T>(asset);
				}
			}
			return nullptr;
		}

		// Add asset
		template<typename T>
		requires std::derived_from<T, IAsset>
		void push(std::shared_ptr<T> _asset)
		{
			if (hasAsset(_asset->getName()))
			{
				log::error("Asset '" + _asset->getName() + "' already exists!");
				return;
			}
			assets.push_back(_asset);
		}

		// Add asset in place
		template<typename T, typename... Args>
		requires std::derived_from<T, IAsset>
		std::shared_ptr<T> emplace(std::string_view _name, Args... args)
		{
			if (hasAsset(_name))
			{
				log::error("Asset '" + _name + "' already exists!");
				return find<T>(_name);
			}
			std::shared_ptr<T> asset = std::make_shared<T>(_name, asyncTaskManager.get(), args...);
			assets.push_back(asset);
			return asset;
		}

	protected:

		std::shared_ptr<AsyncTaskManager> asyncTaskManager;
		std::vector<std::shared_ptr<IAsset>> assets;
	};
}
