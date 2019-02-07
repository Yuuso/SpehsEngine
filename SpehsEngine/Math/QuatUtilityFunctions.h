#pragma once
#include <string>
#include <glm/gtc/quaternion.hpp>

namespace se
{
	class WriteBuffer;
	class ReadBuffer;

	void writeToBuffer(WriteBuffer& writeBuffer, const glm::quat& quat);
	bool readFromBuffer(ReadBuffer& readBuffer, glm::quat& quat);
	std::string toString(const glm::quat& quat);
}
