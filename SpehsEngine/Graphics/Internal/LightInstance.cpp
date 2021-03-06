#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/LightInstance.h"


namespace se
{
	namespace graphics
	{

		LightInstance::LightInstance(Light& _light)
			: light(&_light)
		{
			lightDestroyedConnection = light->destroyedSignal.connect(boost::bind(&LightInstance::lightDestroyed, this));
		}

		bool LightInstance::operator==(const Light& _other) const
		{
			return &_other == light;
		}

		const bool LightInstance::wasDestroyed() const
		{
			return light == nullptr;
		}

		Light& LightInstance::getLight()
		{
			se_assert(light);
			return *light;
		}

		void LightInstance::lightDestroyed()
		{
			light = nullptr;
		}
	}
}
