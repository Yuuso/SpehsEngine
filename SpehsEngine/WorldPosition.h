#pragma once

#define UNIVERSUM_DIAMETER 9223372036854775807

#include <glm\vec2.hpp>


namespace SpehsEngine
{
	struct WorldPosition
	{
		WorldPosition();
		WorldPosition(int64_t, int64_t);
		WorldPosition(WorldPosition _stPos, glm::vec2 _transl);

		glm::vec2 operator-(const WorldPosition& _position);//Returns a vector from position to this
		void operator+=(const WorldPosition& other);//(?)
		void operator+=(const glm::vec2& other);//Add glm vec2 into this
		bool operator==(const WorldPosition& other);
		bool operator!=(const WorldPosition& other);
		void translate(const glm::vec2& _traslation);//Local distance translations
		void translate(int64_t _ix, int64_t _iy);//world translations
		void setPosition(int64_t _ix, int64_t _iy);
		void setPosition(const WorldPosition& other);
		void reset();

		int64_t integerX;
		int64_t integerY;
		float floatingX;
		float floatingY;
	};
}