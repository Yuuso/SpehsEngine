
#pragma once

#include <random>
#include <stdint.h>
//#include <type_traits>
//#include <string>
//#include <assert.h>


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

		//template <typename SeedType>
		//class PRNG
		//{
		//	static_assert(std::is_unsigned<SeedType>::value && (sizeof(SeedType) == 4 || sizeof(SeedType) == 8), "SeedType needs to be either 32bit or 64bit unsigned integer value!");
		//	typedef typename std::conditional<sizeof(SeedType) == 4, std::mt19937, std::mt19937_64>::type MTEngine;

		//public:
		//	PRNG(const SeedType _seed = std::mt19937::default_seed)
		//	{
		//		randomSeed = _seed;
		//		engine.seed(randomSeed);
		//	}

		//	void seed(const SeedType _seed)
		//	{
		//		randomSeed = _seed;
		//		engine.seed(randomSeed);
		//	}
		//	SeedType seed()
		//	{
		//		return randomSeed;
		//	}
		//	void reset()
		//	{
		//		engine.seed(randomSeed);
		//	}
		//	void discard(const unsigned _amount = 1)
		//	{
		//		engine.discard(_amount);
		//	}

		//	template <typename ReturnType>
		//	ReturnType random(const RandomDistribution _distribution = Uniform)
		//	{
		//		static_assert(std::is_arithmetic<ReturnType>::value, "ReturnType needs to be an arithmetic value!");
		//		return random<ReturnType>(std::numeric_limits<ReturnType>::min(), std::numeric_limits<ReturnType>::max(), _distribution);
		//	}

		//	template <typename ReturnType>
		//	ReturnType random(const ReturnType _min, const ReturnType _max, const RandomDistribution _distribution = Uniform)
		//	{
		//		static_assert(std::is_arithmetic<ReturnType>::value, "ReturnType needs to be an arithmetic value!");
		//		switch (_distribution)
		//		{
		//		case Uniform:
		//		{
		//			typedef std::conditional<std::is_integral<ReturnType>::value, std::uniform_int_distribution<ReturnType>, std::uniform_real_distribution<ReturnType>>::type uniform_distribution;
		//			uniform_distribution dist;
		//			return dist(engine, { _min, _max });
		//		}

		//		case Geometric:
		//		{

		//		}
		//		case Pascal:
		//		{

		//		}
		//		case Poisson:
		//		{

		//		}
		//		case Exponential:
		//		{

		//		}
		//		case Gamma:
		//		{

		//		}
		//		case Weibull:
		//		{

		//		}
		//		case ExtremeValue:
		//		{

		//		}
		//		case Normal:
		//		{

		//		}
		//		case Lognormal:
		//		{

		//		}
		//		case ChiSquared:
		//		{

		//		}
		//		case Cauchy:
		//		{

		//		}
		//		case FisherF:
		//		{

		//		}
		//		case StudentT:
		//		{

		//		}
		//		case Discrete:
		//		{

		//		}
		//		case PiecewiseConstant:
		//		{

		//		}
		//		case PiecewiseLinear:
		//		{

		//		}

		//		case Bernoulli:
		//		case Binomial:
		//		default:
		//			assert(false);
		//			break;
		//		}
		//	}

		//	template <typename ReturnType>
		//	ReturnType random(const double _probability, const RandomDistribution _distribution)
		//	{
		//		static_assert(std::is_same<ReturnType, bool>::value, "Invalid return type for Bernoulli random!");

		//		switch (_distribution)
		//		{
		//		case Bernoulli:
		//		{
		//			std::bernoulli_distribution dist(_probability);
		//			return dist(engine);
		//		}
		//		case Binomial:
		//		{
		//			return random<ReturnType>(std::numeric_limits<ReturnType>::min(), std::numeric_limits<ReturnType>::max(), _probability, _distribution);
		//		}

		//		case Geometric:
		//		{

		//		}
		//		case Pascal:
		//		{

		//		}
		//		case Poisson:
		//		{

		//		}
		//		case Exponential:
		//		{

		//		}
		//		case Gamma:
		//		{

		//		}
		//		case Weibull:
		//		{

		//		}
		//		case ExtremeValue:
		//		{

		//		}
		//		case Normal:
		//		{

		//		}
		//		case Lognormal:
		//		{

		//		}
		//		case ChiSquared:
		//		{

		//		}
		//		case Cauchy:
		//		{

		//		}
		//		case FisherF:
		//		{

		//		}
		//		case StudentT:
		//		{

		//		}
		//		case Discrete:
		//		{

		//		}
		//		case PiecewiseConstant:
		//		{

		//		}
		//		case PiecewiseLinear:
		//		{

		//		}

		//		case Uniform:
		//		default:
		//			assert(false);
		//			break;
		//		}
		//	}

		//	template <typename ReturnType>
		//	ReturnType random(const ReturnType _min, const ReturnType _max, const double _probability, const RandomDistribution _distribution)
		//	{
		//		static_assert(std::is_same<ReturnType, bool>::value, "Invalid return type for Bernoulli random!");

		//		switch (_distribution)
		//		{
		//		case Binomial:
		//		{
		//			std::binomial_distribution<ReturnType> dist(_probability, double(_max - _min));
		//			return _min + dist(engine);
		//		}

		//		case Geometric:
		//		{

		//		}
		//		case Pascal:
		//		{

		//		}
		//		case Poisson:
		//		{

		//		}
		//		case Exponential:
		//		{

		//		}
		//		case Gamma:
		//		{

		//		}
		//		case Weibull:
		//		{

		//		}
		//		case ExtremeValue:
		//		{

		//		}
		//		case Normal:
		//		{

		//		}
		//		case Lognormal:
		//		{

		//		}
		//		case ChiSquared:
		//		{

		//		}
		//		case Cauchy:
		//		{

		//		}
		//		case FisherF:
		//		{

		//		}
		//		case StudentT:
		//		{

		//		}
		//		case Discrete:
		//		{

		//		}
		//		case PiecewiseConstant:
		//		{

		//		}
		//		case PiecewiseLinear:
		//		{

		//		}

		//		case Uniform:
		//		case Bernoulli:
		//		default:
		//			assert(false);
		//			break;
		//		}
		//	}

		//private:
		//	SeedType randomSeed;
		//	MTEngine engine;
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