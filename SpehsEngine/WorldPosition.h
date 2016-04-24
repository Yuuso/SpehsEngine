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
		void reset();

		//Buffer read/write
		size_t write(void* buffer);///< Writes world position to buffer. Returns number of bytes written
		size_t read(void* buffer);///< Reads world position from buffer. Returns number of bytes read

		//Operators
		WorldPosition operator+(const glm::vec2& other);
		WorldPosition operator-(const glm::vec2& other);
		WorldPosition operator+(const WorldPosition& other);
		WorldPosition operator-(const WorldPosition& other);
		void operator+=(const glm::vec2& other);
		void operator-=(const glm::vec2& other);
		void operator+=(const WorldPosition& other);
		void operator-=(const WorldPosition& other);
		bool operator==(const WorldPosition& other);
		bool operator!=(const WorldPosition& other);

		//Translation
		void translate(const glm::vec2& _traslation);///< Local distance translations
		void translate(int64_t _ix, int64_t _iy);///< world translation by integral values
		void translate(float fx, float fy);///< Translation in floating point precision
		void setPosition(int64_t _ix, int64_t _iy);
		void setPosition(const WorldPosition& other);

		//Getters
		glm::vec2 getVec2From(const WorldPosition& other);///< Returns a vec2 from other to this
		float getDistance(const WorldPosition& other);
		float getDistanceI(const WorldPosition& other);//Do not measure floating precision, faster calculation

		int64_t integerX;
		int64_t integerY;
		float floatingX;
		float floatingY;
	};
}