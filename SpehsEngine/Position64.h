#pragma once

#define UNIVERSUM_RADIUS 9223372036854775807 //2^63
#define UNIVERSUM_DIAMETER 18446744073709551616 //2^64

#include <glm\vec2.hpp>


namespace spehs
{
	struct Position64
	{
		Position64();
		Position64(int64_t, int64_t);
		Position64(Position64 _stPos, glm::vec2 _transl);
		void reset();

		//Buffer read/write
		size_t write(void* buffer);///< Writes world position to buffer. Returns number of bytes written
		size_t read(void* buffer);///< Reads world position from buffer. Returns number of bytes read

		//Operators
		Position64 operator+(const glm::vec2& other);
		Position64 operator-(const glm::vec2& other);
		Position64 operator+(const Position64& other);
		Position64 operator-(const Position64& other);
		void operator+=(const glm::vec2& other);
		void operator-=(const glm::vec2& other);
		void operator+=(const Position64& other);
		void operator-=(const Position64& other);
		bool operator==(const Position64& other);
		bool operator!=(const Position64& other);

		//Translation
		void translate(const glm::vec2& _traslation);///< Local distance translations
		void translate(int64_t _ix, int64_t _iy);///< world translation by integral values
		void translate(float fx, float fy);///< Translation in floating point precision
		void setPosition(int64_t _ix, int64_t _iy);
		void setPosition(const Position64& other);

		//Getters
		glm::vec2 getVec2From(const Position64& other);///< Returns a vec2 from other to this
		glm::vec2 getVec2To(const Position64& other);///< Returns a vec2 from this to other
		float getDistance(const Position64& other);
		float getDistanceI(const Position64& other);//Do not measure floating precision, faster calculation

		int64_t integerX;
		int64_t integerY;
		float floatingX;
		float floatingY;
	};
}