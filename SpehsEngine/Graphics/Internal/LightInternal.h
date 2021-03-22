#pragma once

#include "boost/signals2/connection.hpp"
#include "SpehsEngine/Graphics/Lights.h"


namespace se
{
	namespace graphics
	{
		class LightInternal
		{
		public:

			LightInternal(Light& _light);
			~LightInternal() = default;

			LightInternal(const LightInternal& _other) = delete;
			LightInternal& operator=(const LightInternal& _other) = delete;

			LightInternal(LightInternal&& _other) = delete;
			LightInternal& operator=(LightInternal&& _other) = delete;

			bool operator==(const Light& _other) const;


			const bool wasDestroyed() const;
			Light& getLight();

		private:

			void lightDestroyed();

			boost::signals2::scoped_connection lightDestroyedConnection;

			Light* light = nullptr;
		};
	}
}
