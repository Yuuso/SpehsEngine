#include "stdafx.h"
#include "SpehsEngine/Math/GLMVectorUtilityFunctions.h"

#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/ReadBuffer.h"

namespace se
{
	void writeToBuffer(WriteBuffer& writeBuffer, const glm::vec2& vec)
	{
		se_write(writeBuffer, vec.x);
		se_write(writeBuffer, vec.y);
	}

	void writeToBuffer(WriteBuffer& writeBuffer, const glm::vec3& vec)
	{
		se_write(writeBuffer, vec.x);
		se_write(writeBuffer, vec.y);
		se_write(writeBuffer, vec.z);
	}

	void writeToBuffer(WriteBuffer& writeBuffer, const glm::vec4& vec)
	{
		se_write(writeBuffer, vec.x);
		se_write(writeBuffer, vec.y);
		se_write(writeBuffer, vec.z);
		se_write(writeBuffer, vec.w);
	}

	void writeToBuffer(WriteBuffer& writeBuffer, const glm::ivec2& vec)
	{
		se_write(writeBuffer, vec.x);
		se_write(writeBuffer, vec.y);
	}

	void writeToBuffer(WriteBuffer& writeBuffer, const glm::ivec3& vec)
	{
		se_write(writeBuffer, vec.x);
		se_write(writeBuffer, vec.y);
		se_write(writeBuffer, vec.z);
	}

	void writeToBuffer(WriteBuffer& writeBuffer, const glm::ivec4& vec)
	{
		se_write(writeBuffer, vec.x);
		se_write(writeBuffer, vec.y);
		se_write(writeBuffer, vec.z);
		se_write(writeBuffer, vec.w);
	}

	bool readFromBuffer(ReadBuffer& readBuffer, glm::vec2& vec)
	{
		se_read(readBuffer, vec.x);
		se_read(readBuffer, vec.y);
		return true;
	}

	bool readFromBuffer(ReadBuffer& readBuffer, glm::vec3& vec)
	{
		se_read(readBuffer, vec.x);
		se_read(readBuffer, vec.y);
		se_read(readBuffer, vec.z);
		return true;
	}

	bool readFromBuffer(ReadBuffer& readBuffer, glm::vec4& vec)
	{
		se_read(readBuffer, vec.x);
		se_read(readBuffer, vec.y);
		se_read(readBuffer, vec.z);
		se_read(readBuffer, vec.w);
		return true;
	}

	bool readFromBuffer(ReadBuffer& readBuffer, glm::ivec2& vec)
	{
		se_read(readBuffer, vec.x);
		se_read(readBuffer, vec.y);
		return true;
	}

	bool readFromBuffer(ReadBuffer& readBuffer, glm::ivec3& vec)
	{
		se_read(readBuffer, vec.x);
		se_read(readBuffer, vec.y);
		se_read(readBuffer, vec.z);
		return true;
	}

	bool readFromBuffer(ReadBuffer& readBuffer, glm::ivec4& vec)
	{
		se_read(readBuffer, vec.x);
		se_read(readBuffer, vec.y);
		se_read(readBuffer, vec.z);
		se_read(readBuffer, vec.w);
		return true;
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
