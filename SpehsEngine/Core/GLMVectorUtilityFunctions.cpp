#include "stdafx.h"
#include "SpehsEngine/Core/GLMVectorUtilityFunctions.h"

#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/ReadBuffer.h"

namespace se
{
	void writeToBuffer(WriteBuffer& writeBuffer, const glm::vec2& vec)
	{
		writeBuffer.write(vec.x);
		writeBuffer.write(vec.y);
	}

	void writeToBuffer(WriteBuffer& writeBuffer, const glm::vec3& vec)
	{
		writeBuffer.write(vec.x);
		writeBuffer.write(vec.y);
		writeBuffer.write(vec.z);
	}

	void writeToBuffer(WriteBuffer& writeBuffer, const glm::vec4& vec)
	{
		writeBuffer.write(vec.x);
		writeBuffer.write(vec.y);
		writeBuffer.write(vec.z);
		writeBuffer.write(vec.w);
	}

	void readFromBuffer(ReadBuffer& readBuffer, glm::vec2& vec)
	{
		readBuffer.read(vec.x);
		readBuffer.read(vec.y);
	}

	void readFromBuffer(ReadBuffer& readBuffer, glm::vec3& vec)
	{
		readBuffer.read(vec.x);
		readBuffer.read(vec.y);
		readBuffer.read(vec.z);
	}

	void readFromBuffer(ReadBuffer& readBuffer, glm::vec4& vec)
	{
		readBuffer.read(vec.x);
		readBuffer.read(vec.y);
		readBuffer.read(vec.z);
		readBuffer.read(vec.w);
	}

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
}
