#pragma once

#include "boost/signals2.hpp"
#include "SpehsEngine/Audio/Internal/InternalTypes.h"
#include "SpehsEngine/Audio/ResourceLoader.h"
#include <future>
#include <memory>
#include <string>


namespace se
{
	namespace audio
	{
		class AudioResource
		{
		public:

			AudioResource() = delete;
			AudioResource(const std::string_view _name);
			~AudioResource();

			AudioResource(const AudioResource& _other) = delete;
			AudioResource& operator=(const AudioResource& _other) = delete;
			AudioResource(AudioResource&& _other) = delete;
			AudioResource& operator=(AudioResource&& _other) = delete;


			const std::string& getName() const;
			bool ready() const;
			void waitUntilReady();
			void reload(std::shared_ptr<ResourceLoader> _resourceLoader);
			bool reloadable() const;
			bool update();
			bool resourceDataIsValid() const;

			template<typename T>
			T* getResource() const
			{
				if (!resourceDataIsValid())
					return nullptr;
				return static_cast<T*>(resourceData.get());
			}

			void setSingleInstance(bool _value); // true: Only one AudioSource can play this at the same time
			void setVolume(float _value);

			float getVolume() const;
			se::time::Time getLength() const;

		private:

			friend class AudioManager;

			static AudioResourceData loadResource(std::string _filename);
			static AudioResourceData streamResource(std::string _filename);
			void load(const std::string& _filename, std::shared_ptr<ResourceLoader> _resourceLoader);
			void stream(const std::string& _filename);
			void applyAttributes();

			bool isStreamResource = false;
			const std::string name;
			std::string path;
			std::future<AudioResourceData> resourceFuture;
			std::shared_ptr<AudioSourceBase> resourceData = nullptr;

			bool singleInstance = false;
			float volume = 1.0f;
			se::time::Time length;

			boost::signals2::signal<void(void)> resourceLoadedSignal;
		};
	}
}
