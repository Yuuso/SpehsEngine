#include "stdafx.h"
#include "SpehsEngine/Core/RandomUtilityFunctions.h"

#include "SpehsEngine/Core/RNG.h"


namespace se
{
	uint32_t getRandomSeed32()
	{
		return rng::random<uint32_t>();
	}
	size_t getRandomIndex(rng::PRNG<unsigned>& _prng, const size_t _size)
	{
		se_assert(_size >= 1);
		return _prng.random<size_t>(0, _size - 1);
	}
	bool getRandomBool(rng::PRNG<unsigned>& _prng)
	{
		return (_prng.random<uint32_t>(0, std::numeric_limits<uint32_t>::max()) % 2) == 0;
	}
	bool getRandomBool(rng::PRNG<unsigned>& _prng, const float _probability)
	{
		return _prng.random<float>(0.0f, 1.0f) < _probability;
	}
	float getRandomFloat(rng::PRNG<unsigned>& _prng, const float _min, const float _max)
	{
		return _prng.random<float>(_min, _max);
	}
	double getRandomDouble(rng::PRNG<unsigned>& _prng, const double _min, const double _max)
	{
		return _prng.random<double>(_min, _max);
	}
	float getRandomRadians(rng::PRNG<unsigned>& _prng)
	{
		return _prng.random<float>(0.0f, TWO_PI<float>);
	}
	glm::vec2 getRandomDirection2D(rng::PRNG<unsigned>& _prng)
	{
		const float radians = _prng.random<float>(0.0f, TWO_PI<float>);
		return glm::vec2(cos(radians), sin(radians));
	}

#define SE_GET_RANDOM_INTEGER(p_Type) \
	p_Type getRandomInteger(rng::PRNG<unsigned>& _prng, const p_Type _min, const p_Type _max) { return _prng.random<p_Type>(_min, _max); }
	SE_GET_RANDOM_INTEGER(int16_t);
	SE_GET_RANDOM_INTEGER(int32_t);
	SE_GET_RANDOM_INTEGER(int64_t);
	SE_GET_RANDOM_INTEGER(uint16_t);
	SE_GET_RANDOM_INTEGER(uint32_t);
	SE_GET_RANDOM_INTEGER(uint64_t);
#undef SE_GET_RANDOM_INTEGER

	Color randomColor(rng::PRNG<unsigned>& _prng)
	{
		return Color(_prng.unit(), _prng.unit(), _prng.unit());
	}
	Color randomColor()
	{
		return randomColor(rng::defaultRandom);
	}
	Color randomBrightColor(rng::PRNG<unsigned>& _prng)
	{
		return colorHSB(_prng.random<float>(0.0f, 360.0f), 1.0f, 0.5f);
	}
	Color randomBrightColor()
	{
		return randomBrightColor(rng::defaultRandom);
	}
}
