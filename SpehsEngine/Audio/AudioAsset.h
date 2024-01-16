#pragma once

#include "SpehsEngine/Audio/Internal/InternalTypes.h"
#include "SpehsEngine/Core/Asset.h"
#include "SpehsEngine/Core/AsyncTaskManager.h"
#include "SpehsEngine/Core/SE_Time.h"


namespace se::audio
{
	enum class LoadType
	{
		Invalid,
		Load,
		Stream
	};

	class AudioAssetData : public AssetData
	{
	public:
		AudioAssetData();
		~AudioAssetData();
		void init(AudioSourceBase* ptr);
		AudioSourceBase* getImpl() const;
		double length = 0.0;
	};

	class AudioAsset : public IAsset
	{
	public:

		~AudioAsset() = default;

		// Construct empty
		AudioAsset(std::string_view _name);

		// Construct and load
		AudioAsset(std::string_view _name, AsyncTaskManager* _taskManager,
			std::string_view _path, LoadType _loadType);

		bool isReloadable() const override;
		void reload(AsyncTaskManager* _taskManager) override;

		// Load or stream audio file
		void load(AsyncTaskManager* _taskManager, std::string_view _path, LoadType _loadType);

		// LoadType
		LoadType getLoadType() const;

		// Volume
		float getVolume() const;
		void setVolume(float _value);

		// Single instance asset can only be played by one source at a time
		bool getSingleInstance() const;
		void setSingleInstance(bool _value);

		// Length
		time::Time getLength() const;

		// Internal audio source
		AudioSourceBase* getSource() const;

	private:

		void handleLoaded() override;
		void applyAttributes();

		LoadType loadType = LoadType::Invalid;
		std::string path;
		bool singleInstance = false;
		float volume = 1.0f;
		se::time::Time length;
	};
}
