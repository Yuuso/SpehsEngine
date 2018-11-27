#pragma once
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace se
{
	class WriteBuffer;
	class ReadBuffer;

	void writeToBuffer(WriteBuffer& writeBuffer, const glm::vec2& vec);
	void writeToBuffer(WriteBuffer& writeBuffer, const glm::vec3& vec);
	void writeToBuffer(WriteBuffer& writeBuffer, const glm::vec4& vec);
	void writeToBuffer(WriteBuffer& writeBuffer, const glm::ivec2& vec);
	void writeToBuffer(WriteBuffer& writeBuffer, const glm::ivec3& vec);
	void writeToBuffer(WriteBuffer& writeBuffer, const glm::ivec4& vec);
	void readFromBuffer(ReadBuffer& readBuffer, glm::vec2& vec);
	void readFromBuffer(ReadBuffer& readBuffer, glm::vec3& vec);
	void readFromBuffer(ReadBuffer& readBuffer, glm::vec4& vec);
	void readFromBuffer(ReadBuffer& readBuffer, glm::ivec2& vec);
	void readFromBuffer(ReadBuffer& readBuffer, glm::ivec3& vec);
	void readFromBuffer(ReadBuffer& readBuffer, glm::ivec4& vec);
	std::string toString(const glm::vec2& vec);
	std::string toString(const glm::vec3& vec);
	std::string toString(const glm::vec4& vec);
	std::string toString(const glm::ivec2& vec);
	std::string toString(const glm::ivec3& vec);
	std::string toString(const glm::ivec4& vec);
}
