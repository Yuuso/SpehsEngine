#include "stdafx.h"
#include "SpehsEngine/Core/Asset.h"


namespace se
{
	IAsset::IAsset(std::string_view _name)
		: name(_name)
	{}
	bool IAsset::isReloadable() const
	{
		return false;
	}
	void IAsset::reload(AsyncTaskManager*)
	{
		// If asset is reloadable, both functions need to be implemented
		se_assert(!isReloadable());
		log::warning("Asset not reloadable!");
	}
	void IAsset::update()
	{
		if (!assetFuture.valid())
			return;
		try
		{
			const std::future_status status = assetFuture.wait_for(std::chrono::seconds(0));
			if (status == std::future_status::ready)
			{
				assetData = assetFuture.get();
				handleLoaded();
				loadedSignal();
			}
		}
		catch (const std::future_error& error)
		{
			if (error.code() == std::make_error_code(std::future_errc::broken_promise))
			{
				// broken_promise can happen if resource loader was deleted before task was completed
				log::warning("Resource loading failed!");
			}
			else
			{
				se_assert_m(false, error.what());
			}
		}
	}
	void IAsset::load(AsyncTaskManager* _taskManager, const std::function<std::shared_ptr<AssetData>()>& _fn)
	{
		if (_taskManager)
		{
			assetFuture = _taskManager->add(_fn);
		}
		else
		{
			assetData = _fn();
			handleLoaded();
			loadedSignal();
		}
	}
	const std::string& IAsset::getName() const
	{
		return name;
	}
	AssetHandle IAsset::getHandle() const
	{
		return assetData->handle;
	}
	bool IAsset::isLoading() const
	{
		return assetFuture.valid();
	}
	bool IAsset::isValid() const
	{
		return assetData != nullptr;
	}
	bool IAsset::getPurgeProtection() const
	{
		return purgeProtection;
	}
	void IAsset::setPurgeProtection(bool value)
	{
		purgeProtection = value;
	}
	void IAsset::waitUntilReady()
	{
		while (isLoading())
		{
			update();
		}
	}
	boost::signals2::scoped_connection IAsset::connectToLoadedSignal(const std::function<void()>& _fn) const
	{
		return loadedSignal.connect(_fn);
	}
}
