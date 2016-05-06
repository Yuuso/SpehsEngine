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
		initialize();
		setSeed(_initSeed);
	}
	RNG::~RNG()
	{

	}

	void RNG::initialize()
	{
		std::random_device randomDevice;
		randomMTEngine.seed(randomDevice());
		seed = randomDevice();
		MTEngine.seed(seed);
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
	unsigned int RNG::mtuirandom()
	{
		return MTEngine();
	}
	int RNG::mtsirandom()
	{
		return intDist(MTEngine, { -INT_MAX, INT_MAX });
	}
	float RNG::mtfrandom()
	{
		return floatDist(MTEngine, { -FLT_MAX, FLT_MAX });
	}
	double RNG::mtdrandom()
	{
		return doubleDist(MTEngine, { -DBL_MAX, DBL_MAX });
	}
	int RNG::mtirandom(int _min, int _max)
	{
		if (_min > _max)
		{
			spehs::console::warning("Min value is greater than Max value! (irandom)");
			return _min;
		}
		if (_min == _max)
			return _min;
		else
			return intDist(MTEngine, { _min, _max });
	}
	float RNG::mtfrandom(float _min, float _max)
	{
		if (_min > _max)
		{
			spehs::console::warning("Min value is greater than Max value! (irandom)");
			return _min;
		}
		if (_min == _max)
			return _min;
		else
			return floatDist(MTEngine, { _min, _max });
	}
	double RNG::mtdrandom(double _min, double _max)
	{
		if (_min > _max)
		{
			spehs::console::warning("Min value is greater than Max value! (irandom)");
			return _min;
		}
		if (_min == _max)
			return _min;
		else
			return doubleDist(MTEngine, { _min, _max });
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
			spehs::console::warning("Min value is greater than Max value! (irandom)");
			return _min;
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
		{
			spehs::console::warning("maxMin value is greater than minMax value! (irandom)");
			return _min;
		}
		if (_maxMin < _min || _minMax > _max)
		{
			spehs::console::warning("Ranges of min-max stuff is wrong!! (irandom)");
			return _min;
		}

		while (value < _minMax && value > _maxMin)
			value = irandom(_min, _max);

		return value;
	}
	float RNG::frandom()
	{
		return floatDist(randomMTEngine, { -FLT_MAX, FLT_MAX });
	}
	float RNG::frandom(float _min, float _max)
	{
		if (_min > _max)
		{
			spehs::console::warning("Min value is greater than Max value! (frandom)");
			return _min;
		}
		return floatDist(randomMTEngine, { _min, _max });
	}	
	float RNG::frandom(float _min, float _max, float _maxMin, float _minMax)
	{
		float value = frandom(_min, _max);
		if (_maxMin >= _minMax)
		{
			spehs::console::warning("maxMin value is greater than minMax value! (frandom)");
			return _min;
		}
		if (_maxMin < _min || _minMax > _max)
		{
			spehs::console::warning("Ranges of min-max stuff is wrong!! (frandom)");
			return _min;
		}

		while (value < _minMax && value > _maxMin)
			value = frandom(_min, _max);

		return value;
	}
	double RNG::drandom()
	{
		return doubleDist(randomMTEngine, { -DBL_MAX, DBL_MAX });
	}
	double RNG::drandom(double _min, double _max)
	{
		if (_min > _max)
		{
			spehs::console::warning("Min value is greater than Max value! (drandom)");
			return _min;
		}
		return doubleDist(randomMTEngine, { _min, _max });
	}
	double RNG::drandom(double _min, double _max, double _maxMin, double _minMax)
	{
		double value = drandom(_min, _max);
		if (_maxMin >= _minMax)
		{
			spehs::console::warning("maxMin value is greater than minMax value! (drandom)");
			return _min;
		}
		if (_maxMin < _min || _minMax > _max)
		{
			spehs::console::warning("Ranges of min-max stuff is wrong!! (drandom)");
			return _min;
		}

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