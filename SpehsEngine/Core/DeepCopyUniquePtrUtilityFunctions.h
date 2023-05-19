#pragma once

#include "SpehsEngine/Core/DeepCopyUniquePtr.h"
#include "SpehsEngine/Core/UniquePtrUtilityFunctions.h"


namespace se
{
	template<typename T> struct SerialTag<DeepCopyUniquePtr<T>> { typedef SerialTagUniquePtr type; };
}
