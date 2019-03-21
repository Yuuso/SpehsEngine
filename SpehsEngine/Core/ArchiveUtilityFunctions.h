#pragma once
#include "SpehsEngine/Core/Archive.h"

namespace se
{
	bool writeToFile(const se::Archive& archive, const std::string& filepath);
	bool readFromFile(se::Archive& archive, const std::string& filepath);
}
