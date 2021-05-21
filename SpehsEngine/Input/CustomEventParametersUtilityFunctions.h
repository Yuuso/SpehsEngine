#pragma once

#include "SpehsEngine/Core/Archive.h"
#include "SpehsEngine/Input/CustomEventParameters.h"


namespace se
{
	Archive writeToArchive(const input::CustomEventParameters& customEventParameters);
	bool readFromArchive(const Archive& archive, input::CustomEventParameters& customEventParameters);
}
