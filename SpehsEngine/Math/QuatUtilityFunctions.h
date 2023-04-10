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

	// Serial
	struct SerialTagQuat {};
	template<typename T, glm::qualifier Q> struct SerialTag<glm::qua<T, Q>> { typedef SerialTagQuat type; };
	template<> template<typename S, typename T>
	static bool Serial<SerialTagQuat>::serial(S& _serial, T _value)
	{
		se_serial(_serial, _value.x, "x");
		se_serial(_serial, _value.y, "y");
		se_serial(_serial, _value.z, "z");
		se_serial(_serial, _value.w, "w");
		return true;
	}
}
