#pragma once


namespace se
{
	uint32_t getRandomSeed32();
	// ]0, size]
	size_t getRandomIndex(rng::PRNG<unsigned>& _prng, const size_t _size);
	// ]0, size]
	size_t getRandomIndex(rng::PRNG<unsigned>& _prng, const size_t _size);
	bool getRandomBool(rng::PRNG<unsigned>& _prng);
	bool getRandomBool(rng::PRNG<unsigned>& _prng, const float _probability);
	float getRandomFloat(rng::PRNG<unsigned>& _prng, const float _min, const float _max);
	double getRandomDouble(rng::PRNG<unsigned>& _prng, const double _min, const double _max);
	float getRandomRadians(rng::PRNG<unsigned>& _prng);
	glm::vec2 getRandomDirection2D(rng::PRNG<unsigned>& _prng);

#define SE_GET_RANDOM_INTEGER(p_Type) \
	p_Type getRandomInteger(rng::PRNG<unsigned>& _prng, const p_Type _min, const p_Type _max);
	SE_GET_RANDOM_INTEGER(int16_t);
	SE_GET_RANDOM_INTEGER(int32_t);
	SE_GET_RANDOM_INTEGER(int64_t);
	SE_GET_RANDOM_INTEGER(uint16_t);
	SE_GET_RANDOM_INTEGER(uint32_t);
	SE_GET_RANDOM_INTEGER(uint64_t);
#undef SE_GET_RANDOM_INTEGER

	Color randomColor(rng::PRNG<unsigned>& _prng);
	Color randomColor();
	Color randomBrightColor(rng::PRNG<unsigned>& _prng);
	Color randomBrightColor();
}
