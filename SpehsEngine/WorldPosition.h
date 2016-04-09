#pragma once

#define UNIVERSUM_RADIUS 9223372036854775807 //2^63
#define UNIVERSUM_DIAMETER 18446744073709551616 //2^64

#include <glm\vec2.hpp>


namespace spehs
{
	struct WorldPosition
	{
		WorldPosition();
		WorldPosition(int64_t, int64_t);
		WorldPosition(WorldPosition _stPos, glm::vec2 _transl);
		size_t write(void* buffer);///< Writes world position to buffer. Returns number of bytes written
		size_t read(void* buffer);///< Reads world position from buffer. Returns number of bytes read
		glm::vec2 operator-(const WorldPosition& _position);///< Returns a vector from position to this
		void operator+=(const WorldPosition& other);//(?)
		void operator+=(const glm::vec2& other);///< Add glm vec2 into this
		bool operator==(const WorldPosition& other);
		bool operator!=(const WorldPosition& other);
		void translate(const glm::vec2& _traslation);///< Local distance translations
		void translate(int64_t _ix, int64_t _iy);///< world translation by integral values
		void setPosition(int64_t _ix, int64_t _iy);
		void setPosition(const WorldPosition& other);
		void reset();

		int64_t integerX;
		int64_t integerY;
		float floatingX;
		float floatingY;
	};
}