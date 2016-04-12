#include <stdlib.h>
#include <time.h>

#include "RNG.h"
#include "Console.h"


spehs::RNG* rng;
namespace spehs
{
	RNG::RNG()
	{
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
		std::random_device randomDevice;
		randomMTEngine.seed(randomDevice());
		seed = randomDevice();
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
	int RNG::mtrandom(int _min, int _max)
	{
		if (_min > _max)
		{
			spehs::console::error("Min value is greater than Max value! (irandom)");
		}
		if (_min == _max)
			return _min;
		else
			return intDist(MTEngine, { _min, _max });
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
		return intDist(randomMTEngine, { -INT_MAX, INT_MAX });
	}
	int RNG::uirandom()
	{
		return intDist(randomMTEngine, { 0, INT_MAX });
	}
	int RNG::irandom(int _min, int _max)
	{
		if (_min > _max)
		{
			spehs::console::error("Min value is greater than Max value! (irandom)");
		}
		if (_min == _max)
			return _min;
		else
			return intDist(randomMTEngine, { _min, _max });
	}
	int RNG::irandom(int _min, int _max, int _maxMin, int _minMax)
	{
		int value = irandom(_min, _max);
		if (_maxMin >= _minMax)
			spehs::console::error("maxMin value is greater than minMax value! (irandom)");
		if (_maxMin < _min || _minMax > _max)
			spehs::console::error("mixMaxMin stuff is wrong!! (irandom)");

		while (value < _minMax && value > _maxMin)
			value = irandom(_min, _max);

		return value;
	}
	float RNG::frandom(float _min, float _max)
	{
		if (_min > _max)
		{
			spehs::console::error("Min value is greater than Max value! (frandom)");
		}
		return floatDist(randomMTEngine, { _min, _max });
	}	
	float RNG::frandom(float _min, float _max, float _maxMin, float _minMax)
	{
		float value = frandom(_min, _max);
		if (_maxMin >= _minMax)
			spehs::console::error("maxMin value is greater than minMax value! (frandom)");
		if (_maxMin < _min || _minMax > _max)
			spehs::console::error("mixMaxMin stuff is wrong!! (frandom)");

		while (value < _minMax && value > _maxMin)
			value = frandom(_min, _max);

		return value;
	}
	double RNG::drandom(double _min, double _max)
	{
		if (_min > _max)
		{
			spehs::console::error("Min value is greater than Max value! (drandom)");
		}
		return doubleDist(randomMTEngine, { _min, _max });
	}
	double RNG::drandom(double _min, double _max, double _maxMin, double _minMax)
	{
		double value = drandom(_min, _max);
		if (_maxMin >= _minMax)
			spehs::console::error("maxMin value is greater than minMax value! (drandom)");
		if (_maxMin < _min || _minMax > _max)
			spehs::console::error("mixMaxMin stuff is wrong!! (drandom)");

		while (value < _minMax && value > _maxMin)
			value = drandom(_min, _max);

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