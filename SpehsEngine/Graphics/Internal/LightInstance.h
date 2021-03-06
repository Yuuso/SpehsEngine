#pragma once

#include "boost/signals2/connection.hpp"
#include "SpehsEngine/Graphics/Lights.h"


namespace se
{
	namespace graphics
	{
		class LightInstance
		{
		public:

			LightInstance(Light& _light);
			~LightInstance() = default;

			LightInstance(const LightInstance& _other) = delete;
			LightInstance& operator=(const LightInstance& _other) = delete;

			LightInstance(LightInstance&& _other) = delete;
			LightInstance& operator=(LightInstance&& _other) = delete;

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
