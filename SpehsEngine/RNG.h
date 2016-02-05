#pragma once

#include <random>


namespace SpehsEngine
{
	class RNG
	{
	public:
		RNG();
		RNG(unsigned int initSeed);

		//Pseudorandom Number Generation = Same seed always returns the same "random" numbers in the same order			
		void setSeed(unsigned int newSeed);
		unsigned int getSeed();
		void resetRandomSeedEngine();
		unsigned int mtrandom(); //returns a large unsigned int from seed
		void discardNext(int amount = 1);
		unsigned int getMax();
		unsigned int getMin();

		//Random Pseudorandom Number Generation
		//Returns only relatively small values (RAND_MAX)
		int sirandom();
		int uirandom();
		int irandom(int min, int max);
		int irandom(int min, int max, int maxMin, int minMax); //Exclude range maxMin <-> minMax
		float frandom(float min, float max);
		float frandom(float min, float max, float maxMin, float minMax); //Exclude range maxMin <-> minMax

	private:
		void initialize();
		unsigned int seed;
		std::mt19937 MTEngine;
	};
}
extern SpehsEngine::RNG* rng;