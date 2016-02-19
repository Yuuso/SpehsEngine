#include <stdlib.h>
#include <time.h>

#include "RNG.h"
#include "Exceptions.h"


SpehsEngine::RNG* rng;
namespace SpehsEngine
{
	RNG::RNG()
	{
		seed = uirandom();
		initialize();
	}
	RNG::RNG(unsigned int _initSeed)
	{
		seed = _initSeed;
		initialize();
	}
	RNG::~RNG()
	{

	}

	void RNG::initialize()
	{
		srand(time(NULL));
		MTEngine = std::mt19937(seed);
	}
	void RNG::setSeed(unsigned int _newSeed)
	{
		seed = _newSeed;
		MTEngine.seed(seed);
	}
	unsigned int RNG::getSeed()
	{
		return seed;
	}
	unsigned int RNG::mtrandom()
	{
		return MTEngine();
	}
	void RNG::discardNext(int _amount)
	{
		MTEngine.discard(_amount);
	}
	unsigned int RNG::getMax()
	{
		return MTEngine.max();
	}
	unsigned int RNG::getMin()
	{
		return MTEngine.min();
	}
	void RNG::resetRandomSeedEngine()
	{
		MTEngine.seed(seed);
	}

	int RNG::sirandom()
	{
		return (rand() % (RAND_MAX / 2 + RAND_MAX / 2)) - RAND_MAX / 2;
	}
	int RNG::uirandom()
	{
		return rand();
	}
	int RNG::irandom(int _min, int _max)
	{
		if (_min > _max)
		{
			SpehsEngine::fatalError("Min value is greater than Max value! (irandom)");
		}
		if (_min == _max)
			return _min;
		else if (_min < 0)
			return (rand() % (abs(_min) + (_max + 1))) - abs(_min);
		else
			return rand() % (_max - _min) + _min;
	}
	int RNG::irandom(int _min, int _max, int _maxMin, int _minMax)
	{
		int value = irandom(_min, _max);
		if (_maxMin >= _minMax)
			SpehsEngine::fatalError("maxMin value is greater than minMax value! (irandom)");
		if (_maxMin < _min || _minMax > _max)
			SpehsEngine::fatalError("mixMaxMin stuff is wrong!! (irandom)");

		while (value < _minMax && value > _maxMin)
			value = irandom(_min, _max);

		return value;
	}
	float RNG::frandom(float _min, float _max)
	{
		if (_min > _max)
		{
			SpehsEngine::fatalError("Min value is greater than Max value! (frandom)");
		}
		float random = ((float) rand() / (float) RAND_MAX);
		float range = _max - _min;
		return (random*range) + _min;
	}	
	float RNG::frandom(float _min, float _max, float _maxMin, float _minMax)
	{
		float value = frandom(_min, _max);
		if (_maxMin >= _minMax)
			SpehsEngine::fatalError("maxMin value is greater than minMax value! (irandom)");
		if (_maxMin < _min || _minMax > _max)
			SpehsEngine::fatalError("mixMaxMin stuff is wrong!! (irandom)");

		while (value < _minMax && value > _maxMin)
			value = frandom(_min, _max);

		return value;
	}


	RNG_64::RNG_64()
	{
		MTEngine = std::mt19937_64();
	}
	RNG_64::RNG_64(uint_fast64_t _seed)
	{
		seed = _seed;
		MTEngine = std::mt19937_64(_seed);
	}
	RNG_64::~RNG_64()
	{

	}

	void RNG_64::setSeed(uint_fast64_t _newSeed)
	{
		seed = _newSeed;
		MTEngine.seed(_newSeed);
	}
	uint_fast64_t RNG_64::getSeed()
	{
		return seed;
	}
	void RNG_64::resetRandomSeedEngine()
	{
		MTEngine.seed(seed);
	}
	uint_fast64_t RNG_64::mtrandom()
	{
		return MTEngine();
	}
	void RNG_64::discardNext(int _amount)
	{
		MTEngine.discard(_amount);
	}
	uint_fast64_t RNG_64::getMax()
	{
		return MTEngine.max();
	}
	uint_fast64_t RNG_64::getMin()
	{
		return MTEngine.min();
	}
}