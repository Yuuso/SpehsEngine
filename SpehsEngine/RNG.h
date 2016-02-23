#pragma once

#include <random>
#include <stdint.h>


namespace SpehsEngine
{
	class RNG
	{
	public:
		RNG();
		RNG(unsigned int _initSeed);
		~RNG();

		//Pseudorandom Number Generation = Same seed always returns the same "random" numbers in the same order			
		void setSeed(unsigned int _newSeed);
		unsigned int getSeed();
		void resetRandomSeedEngine();
		unsigned int mtrandom(); //returns a large unsigned int from seed
		void discardNext(int _amount = 1);
		unsigned int getMax();
		unsigned int getMin();

		//Random Pseudorandom Number Generation
		//Return values from -INT_MAX to INT_MAX
		int sirandom();
		int uirandom();
		int irandom(int _min, int _max);
		int irandom(int _min, int _max, int _maxMin, int _minMax); //Exclude range maxMin <-> minMax
		float frandom(float _min, float _max);
		float frandom(float _min, float _max, float _maxMin, float _minMax); //Exclude range maxMin <-> minMax
		double drandom(double _min, double _max);
		double drandom(double _min, double _max, double _maxMin, double _minMax); //Exclude range maxMin <-> minMax

	private:
		void initialize();
		unsigned int seed;
		std::mt19937 MTEngine; //Used for pseudo-random numbers
		std::mt19937 randomMTEngine; //Used for random numbers

		std::uniform_int_distribution<int> intDist;
		std::uniform_real_distribution<float> floatDist;
		std::uniform_real_distribution<double> doubleDist;
	};

	//64-bit version of the PRNG
	class RNG_64
	{
	public:
		RNG_64();
		RNG_64(uint_fast64_t _seed);
		~RNG_64();

		void setSeed(uint_fast64_t _newSeed);
		uint_fast64_t getSeed();
		void resetRandomSeedEngine();
		uint_fast64_t mtrandom();
		void discardNext(int _amount = 1);
		uint_fast64_t getMax();
		uint_fast64_t getMin();

	private:
		std::mt19937_64 MTEngine;
		uint_fast64_t seed;
	};
}
extern SpehsEngine::RNG* rng;