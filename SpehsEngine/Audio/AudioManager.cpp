#include "stdafx.h"
#include "SpehsEngine/Audio/AudioManager.h"


namespace se
{
	namespace audio
	{
		AudioManager::AudioManager()
		{
			pathFinder = std::make_shared<ResourcePathFinder>();
		}
		AudioManager::~AudioManager()
		{
		}

		void AudioManager::setResourcePathFinder(std::shared_ptr<ResourcePathFinder> _pathFinder)
		{
			pathFinder = _pathFinder;
		}
		void AudioManager::setResourceLoader(std::shared_ptr<ResourceLoader> _resourceLoader)
		{
			resourceLoader = _resourceLoader;
		}

		void AudioManager::update()
		{
			for (auto&& resource : resources)
			{
				resource->update();
			}
		}
		void AudioManager::reload()
		{
			for (auto&& resource : resources)
			{
				if (resource->reloadable())
					resource->reload(resourceLoader);
			}
		}
		void AudioManager::purgeUnused()
		{
			for (size_t i = 0; i < resources.size(); /*i++*/)
			{
				if (resources[i]->reloadable() && resources[i].use_count() == 1)
				{
					resources[i].swap(resources.back());
					resources.pop_back();
				}
				else
				{
					i++;
				}
			}
		}
		bool AudioManager::ready() const
		{
			for (auto&& resource : resources)
			{
				if (!resource->ready())
				{
					return false;
				}
			}
			return true;
		}


		std::shared_ptr<AudioResource> AudioManager::load(const std::string_view _name, const std::string_view _filepath)
		{
			const std::string path = pathFinder->getPath(_filepath);

			for (auto& audio : resources)
			{
				if (audio->getName() == _name)
				{
					log::warning("Cannot create audio resource '" + _name + "', audio resource with that name already exists!");
					if (audio->path != path)
						log::error("Existing audio resource's '" + _name + "' resource path doesn't match!");
					return audio;
				}
			}

			resources.push_back(std::make_shared<AudioResource>(_name));
			std::shared_ptr<AudioResource>& resource = resources.back();
			resource->load(path, resourceLoader);
			return resource;
		}
		std::shared_ptr<AudioResource> AudioManager::stream(const std::string_view _name, const std::string_view _filepath)
		{
			const std::string path = pathFinder->getPath(_filepath);

			for (auto& audio : resources)
			{
				if (audio->getName() == _name)
				{
					log::warning("Cannot create audio resource '" + _name + "', audio resource with that name already exists!");
					if (audio->path != path)
						log::error("Existing audio resource's '" + _name + "' audio resource path doesn't match!");
					return audio;
				}
			}

			resources.push_back(std::make_shared<AudioResource>(_name));
			std::shared_ptr<AudioResource>& resource = resources.back();
			resource->stream(path);
			return resource;
		}
		std::shared_ptr<AudioResource> AudioManager::find(const std::string_view _name) const
		{
			for (auto& resource : resources)
			{
				if (resource->getName() == _name)
					return resource;
			}
			return nullptr;
		}
	}
}
