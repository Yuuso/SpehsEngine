#include "stdafx.h"
#include "SpehsEngine/Math/QuatUtilityFunctions.h"

#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/ReadBuffer.h"

namespace se
{
	void writeToBuffer(WriteBuffer& writeBuffer, const glm::quat& quat)
	{
		se_write(writeBuffer, quat.x);
		se_write(writeBuffer, quat.y);
		se_write(writeBuffer, quat.z);
		se_write(writeBuffer, quat.w);
	}

	bool readFromBuffer(ReadBuffer& readBuffer, glm::quat& quat)
	{
		se_read(readBuffer, quat.x);
		se_read(readBuffer, quat.y);
		se_read(readBuffer, quat.z);
		se_read(readBuffer, quat.w);
		return true;
	}

	std::string toString(const glm::quat& quat)
	{
		return std::string("[" + std::to_string(quat.x) + ", " + std::to_string(quat.y) + ", " + std::to_string(quat.z) + ", " + std::to_string(quat.w) + "]");
	}
}
