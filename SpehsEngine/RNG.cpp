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
	RNG::RNG(unsigned int initSeed) : seed(initSeed)
	{
		seed = initSeed;
		initialize();
	}
	void RNG::initialize()
	{
		srand(time(NULL));
		MTEngine = std::mt19937(seed);
	}

	//Seed-Based Pseudo RNG
	void RNG::setSeed(unsigned int newSeed)
	{
		seed = newSeed;
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

	void RNG::discardNext(int amount)
	{
		MTEngine.discard(amount);
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


	//Time-based RNG
	int RNG::sirandom()
	{
		return (rand() % (RAND_MAX / 2 + RAND_MAX / 2)) - RAND_MAX / 2;
	}

	int RNG::uirandom()
	{
		return rand();
	}

	int RNG::irandom(int min, int max)
	{
		if (min > max)
		{
			SpehsEngine::fatalError("Min value is greater than Max value! (irandom)");
		}
		if (min == max)
			return min;
		else if (min < 0)
			return (rand() % (abs(min) + (max + 1))) - abs(min);
		else
			return rand() % (max - min) + min;
	}

	int RNG::irandom(int min, int max, int maxMin, int minMax)
	{
		int value = irandom(min, max);
		if (maxMin >= minMax)
			SpehsEngine::fatalError("maxMin value is greater than minMax value! (irandom)");
		if (maxMin < min || minMax > max)
			SpehsEngine::fatalError("mixMaxMin stuff is wrong!! (irandom)");

		while (value < minMax && value > maxMin)
			value = irandom(min, max);

		return value;
	}

	float RNG::frandom(float min, float max)
	{
		if (min > max)
		{
			SpehsEngine::fatalError("Min value is greater than Max value! (frandom)");
		}
		float random = ((float) rand() / (float) RAND_MAX);
		float range = max - min;
		return (random*range) + min;
	}
	
	float RNG::frandom(float min, float max, float maxMin, float minMax)
	{
		float value = frandom(min, max);
		if (maxMin >= minMax)
			SpehsEngine::fatalError("maxMin value is greater than minMax value! (irandom)");
		if (maxMin < min || minMax > max)
			SpehsEngine::fatalError("mixMaxMin stuff is wrong!! (irandom)");

		while (value < minMax && value > maxMin)
			value = frandom(min, max);

		return value;
	}
}