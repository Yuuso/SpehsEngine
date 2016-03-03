#include <iostream>

#include "WorldPosition.h"


namespace spehs
{
	WorldPosition::WorldPosition() : integerX(0), integerY(0), floatingX(0.0f), floatingY(0.0f)
	{
	}
	WorldPosition::WorldPosition(int64_t ix, int64_t iy) : integerX(ix), integerY(iy), floatingX(0.0f), floatingY(0.0f)
	{
	}
	WorldPosition::WorldPosition(WorldPosition _stPos, glm::vec2 _transl)
	{
		*this = _stPos;
		translate(_transl);
	}
	glm::vec2 WorldPosition::operator-(const WorldPosition& _position)
	{
		return glm::vec2(integerX - _position.integerX + floatingX - _position.floatingX, integerY - _position.integerY + floatingY - _position.floatingY);
	}
	void WorldPosition::operator+=(const WorldPosition& other) //TODO: ... wtf
	{
		integerX += other.integerX;
		integerY += other.integerY;
	}
	void WorldPosition::operator+=(const glm::vec2& other)
	{
		translate(other);
	}
	bool WorldPosition::operator==(const WorldPosition& other)
	{
		if (integerX != other.integerX)
			return false;
		if (integerY != other.integerY)
			return false;
		if (floatingX != other.floatingX)
			return false;
		if (floatingY != other.floatingY)
			return false;
		return true;
	}
	bool WorldPosition::operator!=(const WorldPosition& other)
	{
		if (integerX != other.integerX)
			return true;
		if (integerY != other.integerY)
			return true;
		if (floatingX != other.floatingX)
			return true;
		if (floatingY != other.floatingY)
			return true;
		return false;
	}
	void WorldPosition::translate(const glm::vec2& _translation)
	{
		//Integer difference
		int64_t dix = std::trunc(_translation.x);
		int64_t diy = std::trunc(_translation.y);

		//Floating difference
		float dfx = _translation.x - dix;
		float dfy = _translation.y - diy;

		//Apply integral movement
		integerX += dix;
		integerY += diy;

		//Apply floating movement
		floatingX += dfx;
		floatingY += dfy;

		//Correct float values
		if (floatingX >= 1.0f)
		{
			floatingX--;
			integerX++;
		}
		else if (floatingX <= -1.0f)
		{
			floatingX++;
			integerX--;
		}
		if (floatingY >= 1.0f)
		{
			floatingY--;
			integerY++;
		}
		else if (floatingY <= -1.0f)
		{
			floatingY++;
			integerY--;
		}
	}

	void WorldPosition::translate(int64_t _ix, int64_t _iy)
	{
		integerX += _ix;
		integerY += _iy;
	}

	void WorldPosition::setPosition(int64_t _ix, int64_t _iy)
	{
		integerX = _ix;
		integerY = _iy;
	}
	void WorldPosition::setPosition(const WorldPosition& other)
	{
		integerX = other.integerX;
		integerY = other.integerY;
		floatingX = other.floatingX;
		floatingY = other.floatingY;
	}
	void WorldPosition::reset()
	{
		integerX = 0;
		integerY = 0;
		floatingX = 0;
		floatingY = 0;
	}

}
