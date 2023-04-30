#pragma once

#include "SpehsEngine/Core/Serial/Serial.h"
#include "SpehsEngine/Core/HeapOptional.h"
#include "SpehsEngine/Core/STLOptionalUtilityFunctions.h"


namespace se
{
	template<typename T, typename D> struct SerialTag<HeapOptional<T, D>> { typedef SerialTagOptional type; };
}
