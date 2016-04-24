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
	void WorldPosition::reset()
	{
		integerX = 0;
		integerY = 0;
		floatingX = 0;
		floatingY = 0;
	}

	//Buffer read/write
	size_t WorldPosition::write(void* buffer)
	{
		memcpy(buffer, &integerX, sizeof(integerX) + sizeof(integerY) + sizeof(floatingX) + sizeof(floatingY));
		return sizeof(integerX) + sizeof(integerY) + sizeof(floatingX) + sizeof(floatingY);
	}
	size_t WorldPosition::read(void* buffer)
	{
		memcpy(&integerX, buffer, sizeof(integerX) + sizeof(integerY) + sizeof(floatingX) + sizeof(floatingY));
		return sizeof(integerX) + sizeof(integerY) + sizeof(floatingX) + sizeof(floatingY);
	}

	//Operators
	WorldPosition WorldPosition::operator+(const glm::vec2& other)
	{
		WorldPosition pos(*this);
		pos += other;
		return pos;
	}
	WorldPosition WorldPosition::operator-(const glm::vec2& other)
	{
		WorldPosition pos(*this);
		pos -= other;
		return pos;
	}
	WorldPosition WorldPosition::operator+(const WorldPosition& other)
	{
		WorldPosition pos(integerX + other.integerX, integerY + other.integerY);
		pos.translate(glm::vec2(floatingX + other.floatingX, floatingY + other.floatingY));
		return pos;
	}
	WorldPosition WorldPosition::operator-(const WorldPosition& other)
	{
		WorldPosition pos(integerX - other.integerX, integerY - other.integerY);
		pos.translate(glm::vec2(floatingX - other.floatingX, floatingY - other.floatingY));
		return pos;
	}
	void WorldPosition::operator-=(const glm::vec2& other)
	{
		translate(-other);
	}
	void WorldPosition::operator+=(const glm::vec2& other)
	{
		translate(other);
	}
	void WorldPosition::operator-=(const WorldPosition& other)
	{
		integerX -= other.integerX;
		integerY -= other.integerY;
		translate(glm::vec2(-other.floatingX, -other.floatingY));
	}
	void WorldPosition::operator+=(const WorldPosition& other)
	{
		integerX += other.integerX;
		integerY += other.integerY;
		translate(glm::vec2(other.floatingX, other.floatingY));
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

	//Translation
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
	void WorldPosition::translate(float fx, float fy)
	{
		//Integer difference
		int64_t dix = std::trunc(fx);
		int64_t diy = std::trunc(fy);

		//Floating difference
		float dfx = fx - dix;
		float dfy = fy - diy;

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

	//Getters
	glm::vec2 WorldPosition::getVec2From(const WorldPosition& _position)
	{
		return glm::vec2(integerX - _position.integerX + floatingX - _position.floatingX, integerY - _position.integerY + floatingY - _position.floatingY);
	}
	float WorldPosition::getDistance(const WorldPosition& other)
	{
		return sqrtf(pow(float(integerX) + floatingX - float(other.integerX) - other.floatingX, 2) + pow(float(integerY) + floatingY - float(other.integerY) - other.floatingY, 2));
	}
	float WorldPosition::getDistanceI(const WorldPosition& other)
	{
		return sqrtf(pow(integerX - other.integerX, 2) + pow(integerY - other.integerY, 2));
	}
}
