#include "stdafx.h"
#include "SpehsEngine/Math/GLMVectorUtilityFunctions.h"

#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/ReadBuffer.h"


namespace se
{
	template<typename T, glm::qualifier Q>
	inline void writeVec2(WriteBuffer& writeBuffer, const glm::vec<2, T, Q>& vec)
	{
		se_write(writeBuffer, vec.x);
		se_write(writeBuffer, vec.y);
	}
	template<typename T, glm::qualifier Q>
	inline void writeVec3(WriteBuffer& writeBuffer, const glm::vec<3, T, Q>& vec)
	{
		se_write(writeBuffer, vec.x);
		se_write(writeBuffer, vec.y);
		se_write(writeBuffer, vec.z);
	}
	template<typename T, glm::qualifier Q>
	inline void writeVec4(WriteBuffer& writeBuffer, const glm::vec<4, T, Q>& vec)
	{
		se_write(writeBuffer, vec.x);
		se_write(writeBuffer, vec.y);
		se_write(writeBuffer, vec.z);
		se_write(writeBuffer, vec.w);
	}

	template<typename T, glm::qualifier Q>
	inline bool readVec2(ReadBuffer& readBuffer, glm::vec<2, T, Q>& vec)
	{
		se_read(readBuffer, vec.x);
		se_read(readBuffer, vec.y);
		return true;
	}
	template<typename T, glm::qualifier Q>
	inline bool readVec3(ReadBuffer& readBuffer, glm::vec<3, T, Q>& vec)
	{
		se_read(readBuffer, vec.x);
		se_read(readBuffer, vec.y);
		se_read(readBuffer, vec.z);
		return true;
	}
	template<typename T, glm::qualifier Q>
	inline bool readVec4(ReadBuffer& readBuffer, glm::vec<4, T, Q>& vec)
	{
		se_read(readBuffer, vec.x);
		se_read(readBuffer, vec.y);
		se_read(readBuffer, vec.z);
		se_read(readBuffer, vec.w);
		return true;
	}

	void writeToBuffer(WriteBuffer& writeBuffer, const glm::u16vec2& vec) { writeVec2<uint16_t, glm::defaultp>(writeBuffer, vec); }
	void writeToBuffer(WriteBuffer& writeBuffer, const glm::u16vec3& vec) { writeVec3<uint16_t, glm::defaultp>(writeBuffer, vec); }
	void writeToBuffer(WriteBuffer& writeBuffer, const glm::u16vec4& vec) { writeVec4<uint16_t, glm::defaultp>(writeBuffer, vec); }
	bool readFromBuffer(ReadBuffer& readBuffer, glm::u16vec2& vec) { return readVec2<uint16_t, glm::defaultp>(readBuffer, vec); }
	bool readFromBuffer(ReadBuffer& readBuffer, glm::u16vec3& vec) { return readVec3<uint16_t, glm::defaultp>(readBuffer, vec); }
	bool readFromBuffer(ReadBuffer& readBuffer, glm::u16vec4& vec) { return readVec4<uint16_t, glm::defaultp>(readBuffer, vec); }

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
