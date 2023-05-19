#include "stdafx.h"
#include "SpehsEngine/Math/QuatUtilityFunctions.h"


namespace se
{
	std::string toString(const glm::quat& quat)
	{
		return std::string("[" + std::to_string(quat.x) + ", " + std::to_string(quat.y) + ", " + std::to_string(quat.z) + ", " + std::to_string(quat.w) + "]");
	}
}
