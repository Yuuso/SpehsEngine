
#pragma once

#include <random>
#include <stdint.h>
//#include <type_traits>
//#include <string>
//
//#include "Exceptions.h"


namespace spehs
{
	namespace rng
	{
		//enum RandomDistribution
		//{
		//	Uniform,
		//	Bernoulli,
		//	Binomial,
		//	Geometric,
		//	Pascal, //Negative Binomial
		//	Poisson,
		//	Exponential,
		//	Gamma,
		//	Weibull,
		//	ExtremeValue,
		//	Normal,
		//	Lognormal,
		//	ChiSquared,
		//	Cauchy,
		//	FisherF,
		//	StudentT,
		//	Discrete,
		//	PiecewiseConstant,
		//	PiecewiseLinear,
		//};

		//template <typename SeedType> //SeedType = Unsigned Integer Type
		//class PRNG
		//{
		//	static_assert(std::is_unsigned(SeedType)::value, "SeedType needs to be an unsigned integer value!");

		//public:
		//	PRNG(const SeedType _seed = std::mt19937::default_seed)
		//	{
		//		switch (sizeof(SeedType))
		//		{
		//		case 4:
		//			randomEngine = new RandomEngine32;
		//			break;
		//		case 8:
		//			randomEngine = new RandomEngine64;
		//			break;
		//		default:
		//			spehs::exceptions::fatalError("Unknown size of SeedType: " + std::to_string(sizeof(SeedType)));
		//			break;
		//		}
		//		randomSeed = _seed;
		//		randomEngine->engine()->seed(randomSeed);
		//	}
		//	~PRNG()
		//	{
		//		delete randomEngine;
		//	}

		//	void seed(const SeedType _seed)
		//	{
		//		randomSeed = _seed;
		//		randomEngine->engine()->seed(_seed);
		//	}
		//	SeedType seed()
		//	{
		//		return randomSeed;
		//	}
		//	void reset()
		//	{
		//		randomEngine->engine()->seed(randomSeed);
		//	}
		//	void discard(const unsigned _amount = 1)
		//	{
		//		randomEngine->engine()->discard(_amount);
		//	}

		//	template <typename ReturnType>
		//	ReturnType operator()(const spehs::rng::RandomDistribution _distribution = Uniform)
		//	{

		//	}
		//	template <typename ReturnType>
		//	ReturnType operator()(const ReturnType _min, const ReturnType _max, const spehs::rng::RandomDistribution _distribution = Uniform)
		//	{

		//	}

		//private:
		//	SeedType randomSeed;
		//	std::enable_if<sizeof(SeedType) == 4, std::mt19937> engine;
		//	std::enable_if<sizeof(SeedType) == 8, std::mt19937_64> engine;
		//};

		//class RandomEngine
		//{
		//	template <typename EngineType>
		//	virtual EngineType* engine()
		//	{
		//		return nullptr;
		//	}
		//};
		//class RandomEngine32 : public RandomEngine
		//{
		//	template <typename EngineType>
		//	EngineType* engine() override
		//	{
		//		return &mtengine;
		//	}
		//private:
		//	std::mt19937 mtengine;
		//};
		//class RandomEngine64 : public RandomEngine
		//{
		//public:
		//	template <typename EngineType>
		//	EngineType* engine() override
		//	{
		//		return &mtengine;
		//	}
		//private:
		//	std::mt19937_64 mtengine;
		//};
		//

		//template <typename ReturnType>
		//ReturnType random(const spehs::rng::RandomDistribution _distribution = Uniform)
		//{

		//}
		//template <typename ReturnType>
		//ReturnType random(const ReturnType _min, const ReturnType _max, const spehs::rng::RandomDistribution _distribution = Uniform)
		//{

		//}










		//Legacy Random Functions
		
		//General true random functions
		int irandom();
		int irandom(const int _min, const int _max);
		unsigned int uirandom();
		unsigned int uirandom(const unsigned int _min, const unsigned int _max);
		unsigned int uirandom(const unsigned int _min, const unsigned int _max, const unsigned int _maxmin, const unsigned int _minmax);
		int sirandom();
		int sirandom(const int _min, const int _max);
		int sirandom(const int _min, const int _max, const int _maxmin, const int _minmax);
		float frandom();
		float frandom(const float _min, const float _max);
		float frandom(const float _min, const float _max, const float _maxmin, const float _minmax);
		double drandom();
		double drandom(const double& _min, const double& _max);
		double drandom(const double& _min, const double& _max, const double& _maxmin, const double& _minmax);
		bool brandom();
		bool coin();
		

		//Pseudo random classes
		class PRNG32
		{
		public:
			PRNG32();
			PRNG32(const unsigned int _initSeed);
			~PRNG32();
		
			void setSeed(const unsigned int _newSeed);
			unsigned int getSeed() const;
			void reset();

			//Uniform distributions
			uint_fast32_t uirandom();
			uint_fast32_t uirandom(const unsigned int _min, const unsigned int _max);
			uint_fast32_t uirandom(const unsigned int _min, const unsigned int _max, const unsigned int _maxmin, const unsigned int _minmax);
			int_fast32_t sirandom();
			int_fast32_t sirandom(const int _min, const int _max);
			int_fast32_t sirandom(const int _min, const int _max, const int _maxmin, const int _minmax);
			float frandom();
			float frandom(const float _min, const float _max);
			float frandom(const float _min, const float _max, const float _maxmin, const float _minmax);
			double drandom();
			double drandom(const double& _min, const double& _max);
			double drandom(const double& _min, const double& _max, const double& _maxmin, const double& _minmax);
			bool brandom();
			
			void discardNext(const int _amount = 1);
			uint_fast32_t getMin() const;
			uint_fast32_t getMax() const;
			
		private:
			unsigned int seed;
			std::mt19937 MTEngine;
		};

		class PRNG64
		{
		public:
			PRNG64();
			PRNG64(const unsigned long long& _seed);
			~PRNG64();

			void setSeed(const unsigned long long& _newSeed);
			unsigned long long getSeed() const;
			void reset();

			uint_fast64_t uirandom();
			uint_fast64_t uirandom(const unsigned long long& _min, const unsigned long long& _max);
			uint_fast64_t uirandom(const unsigned long long& _min, const unsigned long long& _max, const unsigned long long& _maxmin, const unsigned long long& _minmax);
			int_fast64_t sirandom();
			int_fast64_t sirandom(const long long& _min, const long long& _max);
			int_fast64_t sirandom(const long long& _min, const long long& _max, const long long& _maxmin, const long long& _minmax);
			float frandom();
			float frandom(const float _min, const float _max);
			float frandom(const float _min, const float _max, const float _maxmin, const float _minmax);
			double drandom();
			double drandom(const double& _min, const double& _max);
			double drandom(const double& _min, const double& _max, const double& _maxmin, const double& _minmax);
			bool brandom();

			void discardNext(const int _amount = 1);
			uint_fast64_t getMin() const;
			uint_fast64_t getMax() const;

		private:
			unsigned long long seed;
			std::mt19937_64 MTEngine;
		};
	}
}