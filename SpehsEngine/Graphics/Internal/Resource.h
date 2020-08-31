#pragma once

#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Graphics/ResourceData.h"
#include "SpehsEngine/Graphics/Types.h"
#include <chrono>
#include <future>
#include <memory>


namespace se
{
	namespace graphics
	{
		template <typename T>
		class Resource
		{
			static_assert(std::is_base_of<ResourceData, T>::value, "Resource type must be derived from ResourceData!");

		public:

			virtual ~Resource() = default;

			inline bool isReady() const { return !resourceFuture.valid(); }
			virtual void reload(ResourceLoader _resourceLoader) = 0;

		protected:

			void update()
			{
				if (resourceFuture.valid())
				{
					try
					{
						const std::future_status status = resourceFuture.wait_for(std::chrono::seconds(0));
						if (status == std::future_status::ready)
						{
							resourceData = std::dynamic_pointer_cast<T>(resourceFuture.get());
						}
					}
					catch (const std::future_error& error)
					{
						if (error.code() == std::make_error_code(std::future_errc::broken_promise))
						{
							// broken_promise can happen if task manager was deleted before task was completed
							log::warning("Resource loading failed!");
						}
						else
						{
							se_assert_m(false, error.what());
						}
					}
				}
			}

			std::future<std::shared_ptr<ResourceData>> resourceFuture;
			std::shared_ptr<T> resourceData;
		};
	}
}
