#include <iostream>

#include "Position64.h"


namespace spehs
{
	Position64::Position64() : integerX(0), integerY(0), floatingX(0.0f), floatingY(0.0f)
	{
	}
	Position64::Position64(int64_t ix, int64_t iy) : integerX(ix), integerY(iy), floatingX(0.0f), floatingY(0.0f)
	{
	}
	Position64::Position64(Position64 _stPos, glm::vec2 _transl)
	{
		*this = _stPos;
		translate(_transl);
	}
	void Position64::reset()
	{
		integerX = 0;
		integerY = 0;
		floatingX = 0;
		floatingY = 0;
	}

	//Buffer read/write
	size_t Position64::write(void* buffer)
	{
		memcpy(buffer, &integerX, sizeof(integerX) + sizeof(integerY) + sizeof(floatingX) + sizeof(floatingY));
		return sizeof(integerX) + sizeof(integerY) + sizeof(floatingX) + sizeof(floatingY);
	}
	size_t Position64::read(void* buffer)
	{
		memcpy(&integerX, buffer, sizeof(integerX) + sizeof(integerY) + sizeof(floatingX) + sizeof(floatingY));
		return sizeof(integerX) + sizeof(integerY) + sizeof(floatingX) + sizeof(floatingY);
	}

	//Operators
	Position64 Position64::operator+(const glm::vec2& other)
	{
		Position64 pos(*this);
		pos += other;
		return pos;
	}
	Position64 Position64::operator-(const glm::vec2& other)
	{
		Position64 pos(*this);
		pos -= other;
		return pos;
	}
	Position64 Position64::operator+(const Position64& other)
	{
		Position64 pos(integerX + other.integerX, integerY + other.integerY);
		pos.translate(glm::vec2(floatingX + other.floatingX, floatingY + other.floatingY));
		return pos;
	}
	Position64 Position64::operator-(const Position64& other)
	{
		Position64 pos(integerX - other.integerX, integerY - other.integerY);
		pos.translate(glm::vec2(floatingX - other.floatingX, floatingY - other.floatingY));
		return pos;
	}
	void Position64::operator-=(const glm::vec2& other)
	{
		translate(-other);
	}
	void Position64::operator+=(const glm::vec2& other)
	{
		translate(other);
	}
	void Position64::operator-=(const Position64& other)
	{
		integerX -= other.integerX;
		integerY -= other.integerY;
		translate(glm::vec2(-other.floatingX, -other.floatingY));
	}
	void Position64::operator+=(const Position64& other)
	{
		integerX += other.integerX;
		integerY += other.integerY;
		translate(glm::vec2(other.floatingX, other.floatingY));
	}
	bool Position64::operator==(const Position64& other)
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
	bool Position64::operator!=(const Position64& other)
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
	void Position64::translate(const glm::vec2& _translation)
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
	void Position64::translate(float fx, float fy)
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
	void Position64::translate(int64_t _ix, int64_t _iy)
	{
		integerX += _ix;
		integerY += _iy;
	}
	void Position64::setPosition(int64_t _ix, int64_t _iy)
	{
		integerX = _ix;
		integerY = _iy;
	}
	void Position64::setPosition(const Position64& other)
	{
		integerX = other.integerX;
		integerY = other.integerY;
		floatingX = other.floatingX;
		floatingY = other.floatingY;
	}

	//Getters
	glm::vec2 Position64::getVec2From(const Position64& _position)
	{
		return glm::vec2(integerX - _position.integerX + floatingX - _position.floatingX, integerY - _position.integerY + floatingY - _position.floatingY);
	}
	glm::vec2 Position64::getVec2To(const Position64& _position)
	{
		return glm::vec2(_position.integerX - integerX + _position.floatingX - floatingX, _position.integerY - integerY + _position.floatingY - floatingY);
	}
	float Position64::getDistance(const Position64& other)
	{
		return sqrtf(pow(float(integerX) + floatingX - float(other.integerX) - other.floatingX, 2) + pow(float(integerY) + floatingY - float(other.integerY) - other.floatingY, 2));
	}
	float Position64::getDistanceI(const Position64& other)
	{
		return sqrtf(pow(integerX - other.integerX, 2) + pow(integerY - other.integerY, 2));
	}
}
