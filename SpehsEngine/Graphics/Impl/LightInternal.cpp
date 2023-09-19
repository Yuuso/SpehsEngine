#include "stdafx.h"
#include "SpehsEngine/Graphics/Impl/LightInternal.h"

#include "SpehsEngine/Graphics/Lights.h"


LightInternal::LightInternal(Light& _light)
	: light(&_light)
{
	lightDestroyedConnection = light->destroyedSignal.connect(boost::bind(&LightInternal::lightDestroyed, this));
}

bool LightInternal::operator==(const Light& _other) const
{
	return &_other == light;
}

const bool LightInternal::wasDestroyed() const
{
	return light == nullptr;
}

Light& LightInternal::getLight()
{
	se_assert(light);
	return *light;
}

void LightInternal::lightDestroyed()
{
	light = nullptr;
}
