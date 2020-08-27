#pragma once

#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Graphics/Types.h"
#include <chrono>
#include <future>
#include <memory>


namespace se
{
	namespace graphics
	{
		class Resource
		{
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
							resourceHandle = resourceFuture.get();
						}
					}
					catch (const std::future_error & error)
					{
						if (error.code() == std::make_error_code(std::future_errc::broken_promise))
						{
							// broken_promise can happen if task manager was deleted before task was completed
							log::warning("Texture resource loading failed!");
						}
						else
						{
							se_assert_m(false, error.what());
						}
					}
				}
			}

			std::future<ResourceHandle> resourceFuture;
			ResourceHandle resourceHandle = INVALID_RESOURCE_HANDLE;
		};
	}
}
