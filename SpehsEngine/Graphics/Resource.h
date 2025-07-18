#pragma once

#include "SpehsEngine/Graphics/ResourceData.h"
#include "SpehsEngine/Graphics/ResourceLoader.h"
#include <future>


namespace se
{
	namespace graphics
	{
		template <typename T>
		class Resource
		{
			static_assert(std::is_base_of<ResourceData, T>::value, "Resource type must be derived from ResourceData!");

		public:

			Resource(const std::string_view _name) : name(_name) {}
			virtual ~Resource() = default;

			inline const std::string& getName() const { return name; }
			inline bool ready() const { return !resourceFuture.valid(); }
			inline void waitUntilReady()
			{
				while (!ready())
					update();
			}

			virtual void reload(std::shared_ptr<ResourceLoader> _resourceLoader) = 0;
			virtual bool reloadable() const = 0;
			virtual bool update()
			{
				if (resourceFuture.valid())
				{
					try
					{
						const std::future_status status = resourceFuture.wait_for(std::chrono::seconds(0));
						if (status == std::future_status::ready)
						{
							resourceData = std::dynamic_pointer_cast<T>(resourceFuture.get());
							resourceLoadedSignal();
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

			bool resourceDataIsValid() const
			{
				return resourceData != nullptr;
			}
			ResourceHandle getHandle() const
			{
				if (!resourceDataIsValid())
					return INVALID_RESOURCE_HANDLE;
				return resourceData->handle;
			}

		protected:

			const std::string name;
			std::future<std::shared_ptr<ResourceData>> resourceFuture;
			std::shared_ptr<T> resourceData;

			Signal<void(void)> resourceLoadedSignal;
		};
	}
}
