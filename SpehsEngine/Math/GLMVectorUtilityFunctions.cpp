#include "stdafx.h"
#include "SpehsEngine/Math/GLMVectorUtilityFunctions.h"


namespace se
{
	std::string toString(const glm::vec2& vec)
	{
		return std::string("[" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + "]");
	}

	std::string toString(const glm::vec3& vec)
	{
		return std::string("[" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + "]");
	}

	std::string toString(const glm::vec4& vec)
	{
		return std::string("[" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ", " + std::to_string(vec.w) + "]");
	}

	std::string toString(const glm::ivec2& vec)
	{
		return std::string("[" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + "]");
	}

	std::string toString(const glm::ivec3& vec)
	{
		return std::string("[" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + "]");
	}

	std::string toString(const glm::ivec4& vec)
	{
		return std::string("[" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ", " + std::to_string(vec.w) + "]");
	}
}
