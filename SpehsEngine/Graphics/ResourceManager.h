#pragma once

#include "SpehsEngine/Core/ResourcePathFinder.h"
#include "SpehsEngine/Graphics/Resource.h"
#include "SpehsEngine/Graphics/ResourceLoader.h"


namespace se
{
	namespace graphics
	{
		template <typename T>
		class ResourceManager
		{
		public:

			ResourceManager()
			{
				pathFinder = std::make_shared<ResourcePathFinder>();
			}
			virtual ~ResourceManager() = default;

			ResourceManager(const ResourceManager& _other) = delete;
			ResourceManager& operator=(const ResourceManager& _other) = delete;

			ResourceManager(ResourceManager&& _other) = delete;
			ResourceManager& operator=(ResourceManager&& _other) = delete;


			void setResourcePathFinder(std::shared_ptr<ResourcePathFinder> _pathFinder)
			{
				pathFinder = _pathFinder;
			}
			void setResourceLoader(std::shared_ptr<ResourceLoader> _resourceLoader)
			{
				resourceLoader = _resourceLoader;
			}

			void update()
			{
				for (auto&& resource : resources)
				{
					resource->update();
				}
			}
			void reload()
			{
				for (auto&& resource : resources)
				{
					if (resource->reloadable())
						resource->reload(resourceLoader);
				}
			}
			void purgeUnused()
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
			bool ready() const
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

		protected:

			std::shared_ptr<ResourcePathFinder> pathFinder = nullptr;
			std::shared_ptr<ResourceLoader> resourceLoader = nullptr;

			std::vector<std::shared_ptr<T>> resources;
		};
	}
}
