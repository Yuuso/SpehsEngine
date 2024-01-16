#pragma once

#include "SpehsEngine/Core/AsyncTaskManager.h"


namespace se
{
	// Asset handle union for implementation data.
	union AssetHandle
	{
		AssetHandle() = default;
		AssetHandle(void* _ptr) : ptr(_ptr){}
		AssetHandle(uint16_t _handle) : u16handle(_handle){}

		void* ptr;
		uint16_t u16handle;
	};
	static_assert(sizeof(AssetHandle) == sizeof(void*));


	// Basic class for asset data.
	// Derived classes can include custom data for internal use.
	class AssetData
	{
	public:
		SE_NO_COPY_OR_MOVE(AssetData);
		AssetData() = default;
		virtual ~AssetData() = default;
		AssetHandle handle;
	};


	// Interface class for assets.
	// All assets should implement the following constructors:
	//	Asset(string_view, AsyncTaskManager*, ...)
	//	(See AssetManager::emplace)
	class SE_INTERFACE IAsset
	{
	public:

		SE_NO_COPY_OR_MOVE(IAsset);
		IAsset(std::string_view _name);
		virtual ~IAsset() = default;

		// By default assets are not reloadable.
		// If an asset is reloadable it should implement the reload & isReloadable functions.
		virtual bool isReloadable() const;
		virtual void reload(AsyncTaskManager*);

		// Update async loading status.
		void update();

		// Handle sync/async loading & callbacks.
		void load(AsyncTaskManager* _taskManager, const std::function<std::shared_ptr<AssetData>()>& _fn);

		// Asset name, provided in constructor.
		const std::string& getName() const;

		// Handle for internal use.
		AssetHandle getHandle() const;

		// Data for internal use
		template<typename T>
		requires std::derived_from<T, AssetData>
		const T* getData() const
		{
			return assetData
				? std::dynamic_pointer_cast<T>(assetData).get()
				: nullptr;
		}

		// Returns true if loading.
		bool isLoading() const;

		// Returns true if asset data is valid.
		// NOTE: Could be reloading even if valid.
		bool isValid() const;

		// Purge protected assets won't be removed in AssetManager::purgeUnused().
		bool getPurgeProtection() const;
		void setPurgeProtection(bool value);

		// If loading: Blocks until ready.
		void waitUntilReady();

		// Connect to 'loaded' signal.
		boost::signals2::scoped_connection connectToLoadedSignal(const std::function<void()>& _fn) const;

	protected:

		// Called after loading finishes, before 'loaded' signal.
		virtual void handleLoaded() {}

	private:

		const std::string name;
		std::future<std::shared_ptr<AssetData>> assetFuture;
		std::shared_ptr<AssetData> assetData;
		bool purgeProtection = false;
		mutable boost::signals2::signal<void()> loadedSignal;
	};
}
