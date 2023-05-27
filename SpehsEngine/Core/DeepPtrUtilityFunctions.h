#pragma once

#include "SpehsEngine/Core/DeepPtr.h"
#include "SpehsEngine/Core/UniquePtrUtilityFunctions.h"


namespace se
{
	template<typename T> struct SerialTag<DeepPtr<T>> { typedef SerialTagUniquePtr type; };
}
