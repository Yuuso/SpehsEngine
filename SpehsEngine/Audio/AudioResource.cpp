#include "stdafx.h"
#include "SpehsEngine/Audio/AudioResource.h"

#include "soloud/soloud_wav.h"
#include "soloud/soloud_wavstream.h"
#include "SpehsEngine/Audio/Internal/GlobalBackend.h"


namespace se
{
	namespace audio
	{
		AudioResource::AudioResource(const std::string_view _name)
			: name(_name)
		{
		}
		AudioResource::~AudioResource()
		{
		}

		const std::string& AudioResource::getName() const
		{
			return name;
		}
		bool AudioResource::ready() const
		{
			return !resourceFuture.valid();
		}
		void AudioResource::waitUntilReady()
		{
			while (!ready())
				update();
		}

		std::shared_ptr<AudioSourceBase> AudioResource::loadResource(std::string _filename)
		{
			auto result = std::make_shared<SoLoud::Wav>();
			result->load(_filename.c_str());
			if (globalDefaultEnableDistanceDelay)
				result->set3dDistanceDelay(true);
			return result;
		}
		std::shared_ptr<AudioSourceBase> AudioResource::streamResource(std::string _filename)
		{
			auto result = std::make_shared<SoLoud::WavStream>();
			result->load(_filename.c_str());
			if (globalDefaultEnableDistanceDelay)
				result->set3dDistanceDelay(true);
			return result;
		}
		void AudioResource::load(const std::string& _filename, std::shared_ptr<ResourceLoader> _resourceLoader)
		{
			path = _filename;
			if (_resourceLoader)
			{
				resourceFuture = _resourceLoader->push([this]{ return loadResource(path); });
			}
			else
			{
				resourceData = loadResource(path);
				applyAttributes();
			}
		}
		void AudioResource::stream(const std::string& _filename)
		{
			path = _filename;
			resourceData = streamResource(path);
			applyAttributes();
		}
		void AudioResource::reload(std::shared_ptr<ResourceLoader> _resourceLoader)
		{
			if (!reloadable())
			{
				log::warning("Cannot reload AudioResource!");
				return;
			}
			if (resourceData)
			{
				getResource<SoLoud::AudioSource>()->stop();
				resourceData.reset();
			}
			load(path, _resourceLoader);
		}
		bool AudioResource::reloadable() const
		{
			return !path.empty();
		}
		bool AudioResource::update()
		{
			if (resourceFuture.valid())
			{
				try
				{
					const std::future_status status = resourceFuture.wait_for(std::chrono::seconds(0));
					if (status == std::future_status::ready)
					{
						resourceData = resourceFuture.get();
						resourceLoadedSignal();
						applyAttributes();
						return true;
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
			return false;
		}

		bool AudioResource::resourceDataIsValid() const
		{
			return resourceData != nullptr;
		}

		void AudioResource::applyAttributes()
		{
			if (auto data = getResource<SoLoud::AudioSource>())
			{
				data->setSingleInstance(singleInstance);
				data->setVolume(volume);
			}
		}

		void AudioResource::setSingleInstance(bool _value)
		{
			singleInstance = _value;
			if (auto data = getResource<SoLoud::AudioSource>())
			{
				data->setSingleInstance(singleInstance);
			}
		}
		void AudioResource::setVolume(float _value)
		{
			volume = _value;
			if (auto data = getResource<SoLoud::AudioSource>())
			{
				data->setVolume(volume);
			}
		}

		bool AudioResource::isSingleInstance() const
		{
			return singleInstance;
		}
		float AudioResource::getVolume() const
		{
			return volume;
		}
	}
}
