#include "stdafx.h"
#include "SpehsEngine/Audio/AudioAsset.h"

#include "soloud/soloud_wav.h"
#include "soloud/soloud_wavstream.h"
#include "SpehsEngine/Audio/Internal/GlobalBackend.h"


namespace se::audio
{
	static std::shared_ptr<AudioAssetData> loadAudioAsset(std::string _path, LoadType _loadType)
	{
		std::shared_ptr<AudioAssetData> result = std::make_shared<AudioAssetData>();
		switch (_loadType)
		{
			default:
				log::error("Unknown LoadType!");
				[[fallthrough]];
			case LoadType::Load:
			{
				SoLoud::Wav* source = new SoLoud::Wav();
				result->init(source);
				SoLoud::result loadResult = source->load(_path.c_str());
				if (loadResult != 0)
				{
					log::error("Failed to load audio asset ("
						+ _path + "): " + globalSoloud->getErrorString(loadResult));
					return result;
				}
				result->length = source->getLength();
				break;
			}
			case LoadType::Stream:
			{
				SoLoud::WavStream* source = new SoLoud::WavStream();
				result->init(source);
				SoLoud::result loadResult = source->load(_path.c_str());
				if (loadResult != 0)
				{
					log::error("Failed to stream audio asset ("
						+ _path + "): " + globalSoloud->getErrorString(loadResult));
					return result;
				}
				result->length = source->getLength();
				break;
			}
		}

		if (globalDefaultEnableDistanceDelay)
		{
			result->getImpl()->set3dDistanceDelay(true);
		}
		return result;
	}

	AudioAssetData::AudioAssetData()
	{
		handle.ptr = nullptr;
	}
	AudioAssetData::~AudioAssetData()
	{
		AudioSourceBase* ptr = getImpl();
		if (ptr)
		{
			ptr->stop();
			delete ptr;
		}
	}
	void AudioAssetData::init(AudioSourceBase* ptr)
	{
		se_assert(ptr);
		handle.ptr = static_cast<void*>(ptr);
	}
	AudioSourceBase* AudioAssetData::getImpl() const
	{
		return static_cast<AudioSourceBase*>(handle.ptr);
	}

	AudioAsset::AudioAsset(std::string_view _name)
		: IAsset(_name)
	{}
	AudioAsset::AudioAsset(std::string_view _name, AsyncTaskManager* _taskManager,
			std::string_view _path, LoadType _loadType)
		: IAsset(_name)
	{
		load(_taskManager, _path, _loadType);
	}
	bool AudioAsset::isReloadable() const
	{
		return !path.empty() && loadType != LoadType::Invalid;
	}
	void AudioAsset::reload(AsyncTaskManager* _taskManager)
	{
		if (!isReloadable())
		{
			log::warning("AudioAsset not reloadable!");
			return;
		}
		load(_taskManager, path, loadType);
	}
	void AudioAsset::load(AsyncTaskManager* _taskManager, std::string_view _path, LoadType _loadType)
	{
		path = _path;
		loadType = _loadType;
		IAsset::load(_taskManager, [this]{ return loadAudioAsset(path, loadType); });
	}
	LoadType AudioAsset::getLoadType() const
	{
		return loadType;
	}
	float AudioAsset::getVolume() const
	{
		return volume;
	}
	void AudioAsset::setVolume(float _value)
	{
		volume = _value;
		if (AudioSourceBase* data = getSource())
		{
			data->setVolume(volume);
		}
	}
	bool AudioAsset::getSingleInstance() const
	{
		return singleInstance;
	}
	void AudioAsset::setSingleInstance(bool _value)
	{
		singleInstance = _value;
		if (AudioSourceBase* data = getSource())
		{
			data->setSingleInstance(singleInstance);
		}
	}
	time::Time AudioAsset::getLength() const
	{
		return length;
	}
	AudioSourceBase* AudioAsset::getSource() const
	{
		if (!isValid())
			return nullptr;
		const AudioAssetData* ptr = getData<AudioAssetData>();
		return ptr->getImpl();
	}
	void AudioAsset::handleLoaded()
	{
		if (!isValid())
			return;
		const AudioAssetData* ptr = getData<AudioAssetData>();
		length = se::time::fromSeconds(ptr->length);
		applyAttributes();
	}
	void AudioAsset::applyAttributes()
	{
		if (AudioSourceBase* data = getSource())
		{
			data->setSingleInstance(singleInstance);
			data->setVolume(volume);
		}
	}
}
