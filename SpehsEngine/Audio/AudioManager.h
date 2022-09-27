#pragma once

#include "SpehsEngine/Audio/AudioResource.h"
#include "SpehsEngine/Audio/ResourceLoader.h"
#include "SpehsEngine/Core/ResourcePathFinder.h"
#include <memory>
#include <string>


namespace se
{
	namespace audio
	{
		class AudioManager
		{
		public:

			AudioManager();
			~AudioManager();

			AudioManager(const AudioManager& _other) = delete;
			AudioManager& operator=(const AudioManager& _other) = delete;

			AudioManager(AudioManager&& _other) = delete;
			AudioManager& operator=(AudioManager&& _other) = delete;


			void setResourcePathFinder(std::shared_ptr<ResourcePathFinder> _pathFinder);
			void setResourceLoader(std::shared_ptr<ResourceLoader> _resourceLoader);

			void update();
			void reload();
			void purgeUnused();
			bool ready() const;


			std::shared_ptr<AudioResource> load(const std::string_view _name, const std::string_view _filepath);
			std::shared_ptr<AudioResource> stream(const std::string_view _name, const std::string_view _filepath);
			std::shared_ptr<AudioResource> find(const std::string_view _name) const;

		private:

			std::shared_ptr<ResourcePathFinder> pathFinder = nullptr;
			std::shared_ptr<ResourceLoader> resourceLoader = nullptr;

			std::vector<std::shared_ptr<AudioResource>> resources;
		};
	}
}
