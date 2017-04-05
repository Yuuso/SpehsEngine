
#pragma once

#include <glm/vec2.hpp>

#include <random>
#include <stdint.h>
#include <type_traits>
#include <assert.h>
#include <mutex>

#define PI 3.14159265358f


namespace spehs
{
	namespace rng
	{
		namespace Distribution
		{
			//Uniform Distributions
			template<typename type>
			using UniformInt = std::uniform_int_distribution<type>;

			template<typename type>
			using UniformReal = std::uniform_real_distribution<type>;


			//Bernoulli Distributions
			using Bernoulli = std::bernoulli_distribution;

			template<typename type>
			using Geometric = std::geometric_distribution<type>;

			template<typename type>
			using Binomial = std::binomial_distribution<type>;

			template<typename type>
			using Pascal = std::negative_binomial_distribution<type>;


			//Poisson Distirbutions
			template<typename type>
			using Poisson = std::poisson_distribution<type>;

			template<typename type>
			using Exponential = std::exponential_distribution<type>;

			template<typename type>
			using Gamma = std::gamma_distribution<type>;

			template<typename type>
			using Weibull = std::weibull_distribution<type>;

			template<typename type>
			using ExtremeValue = std::extreme_value_distribution<type>;


			//Normal Distributions
			template<typename type>
			using Normal = std::normal_distribution<type>;

			template<typename type>
			using LogNormal = std::lognormal_distribution<type>;

			template<typename type>
			using ChiSquared = std::chi_squared_distribution<type>;

			template<typename type>
			using Cauchy = std::cauchy_distribution<type>;

			template<typename type>
			using FisherF = std::fisher_f_distribution<type>;

			template<typename type>
			using StudentT = std::student_t_distribution<type>;


			//Sampling Distributions (Note: These probably won't work since there is no method with an appropriate parameter list!)
			template<typename type>
			using Discrete = std::discrete_distribution<type>;

			template<typename type>
			using PiecewiseConstant = std::piecewise_constant_distribution<type>;

			template<typename type>
			using PiecewiseLinear = std::piecewise_linear_distribution<type>;
		}

		//General 'true' random functions
		extern std::mutex rngmutex;

		template <typename ReturnType, typename DistributionType =
			std::conditional<std::is_floating_point<ReturnType>::value, Distribution::UniformReal<ReturnType>, Distribution::UniformInt<ReturnType>>::type>
		typename std::enable_if<sizeof(ReturnType) != 1, ReturnType>::type random()
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return defaultRandom.random<ReturnType, DistributionType>(std::numeric_limits<ReturnType>::min(), std::numeric_limits<ReturnType>::max());
		}

		template <typename ReturnType, typename DistributionType = Distribution::UniformInt<int>>
			typename std::enable_if<sizeof(ReturnType) == 1, ReturnType>::type random()
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return (ReturnType)defaultRandom.random<int, DistributionType>(std::numeric_limits<ReturnType>::min(), std::numeric_limits<ReturnType>::max());
		}

		template <typename ReturnType, typename DistributionType =
			std::conditional<std::is_floating_point<ReturnType>::value, Distribution::UniformReal<ReturnType>, Distribution::UniformInt<ReturnType>>::type>
			typename std::enable_if<sizeof(ReturnType) != 1, ReturnType>::type random(const ReturnType _min, const ReturnType _max)
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return defaultRandom.random<ReturnType, DistributionType>(_min, _max);
		}

		template <typename ReturnType, typename DistributionType = Distribution::UniformInt<int>>
			typename std::enable_if<sizeof(ReturnType) == 1, ReturnType>::type random(const ReturnType _min, const ReturnType _max)
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return (ReturnType) defaultRandom.random<int, DistributionType>((int) _min, (int) _max);
		}

		template <typename ReturnType, typename DistributionType = Distribution::Bernoulli>
		ReturnType random(const double _probability)
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return defaultRandom.random<ReturnType, DistributionType>(_probability);
		}

		template <typename ReturnType, typename DistributionType = Distribution::Binomial<ReturnType>>
		ReturnType random(const double _probability, const ReturnType _min, const ReturnType _max)
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return defaultRandom.random<ReturnType, DistributionType>(_probability, _min, _max);
		}

		template<typename FloatingPointType = float>
		FloatingPointType unit()
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return defaultRandom.unit<FloatingPointType>();
		}
		template<typename FloatingPointType = float>
		FloatingPointType angle()
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return defaultRandom.angle<FloatingPointType>();
		}
		bool coin();
		bool weightedCoin(const double _trueProbability = 0.5);
		unsigned dice(const unsigned _sides);
		unsigned weightedDice(const std::initializer_list<double> _propabilities);

		
		//Pseudo random number generator class
		template <typename SeedType>
		class PRNG
		{
			static_assert(std::is_unsigned<SeedType>::value && (sizeof(SeedType) == 4 || sizeof(SeedType) == 8), "SeedType needs to be either 32bit or 64bit unsigned integer value!");
			typedef typename std::conditional<sizeof(SeedType) == 4, std::mt19937, std::mt19937_64>::type MTEngine;

		public:
			PRNG(const SeedType _seed = std::mt19937::default_seed)
			{
				randomSeed = _seed;
				engine.seed(randomSeed);
			}

			void seed(const SeedType _seed)
			{
				randomSeed = _seed;
				engine.seed(randomSeed);
			}
			void seed(const std::seed_seq& _seed)
			{
				//Maybe needs some clarification later...
				randomSeed = 0;
				engine.seed(randomSeed);
			}
			SeedType seed()
			{
				return randomSeed;
			}
			void reset()
			{
				engine.seed(randomSeed);
			}
			void discard(const unsigned _amount = 1)
			{
				engine.discard(_amount);
			}

			//Default random functions
			template <typename ReturnType, typename DistributionType = 
				std::conditional<std::is_floating_point<ReturnType>::value, Distribution::UniformReal<ReturnType>, Distribution::UniformInt<ReturnType>>::type>
			typename std::enable_if<sizeof(ReturnType) != 1, ReturnType>::type random()
			{
				return random<ReturnType>(std::numeric_limits<ReturnType>::min(), std::numeric_limits<ReturnType>::max());
			}

			template <typename ReturnType, typename DistributionType = Distribution::UniformInt<int>>
			typename std::enable_if<sizeof(ReturnType) == 1, ReturnType>::type random()
			{
				return (ReturnType) random<int>(std::numeric_limits<ReturnType>::min(), std::numeric_limits<ReturnType>::max());
			}

			template <typename ReturnType, typename DistributionType = 
				std::conditional<std::is_integral<ReturnType>::value, Distribution::UniformInt<ReturnType>, Distribution::UniformReal<ReturnType>>::type>
				typename std::enable_if<sizeof(ReturnType) != 1, ReturnType>::type random(const ReturnType _min, const ReturnType _max)
			{
				static_assert(std::is_arithmetic<ReturnType>::value, "ReturnType needs to be an arithmetic value!");
				assert(_min <= _max);
				DistributionType dist;
				return dist(engine, { _min, _max });
			}

			template <typename ReturnType, typename DistributionType = Distribution::UniformInt<int>>
				typename std::enable_if<sizeof(ReturnType) == 1, ReturnType>::type random(const ReturnType _min, const ReturnType _max)
			{
				static_assert(std::is_arithmetic<ReturnType>::value, "ReturnType needs to be an arithmetic value!");
				assert(_min <= _max);
				DistributionType dist;
				return dist(engine, { _min, _max });
			}

			//Functions for certain distirbutions
			template <typename ReturnType, typename DistributionType = Distribution::Bernoulli>
			ReturnType random(const double _probability)
			{
				static_assert(std::is_arithmetic<ReturnType>::value, "ReturnType needs to be an arithmetic value!");
				DistributionType dist(_probability);
				return dist(engine);
			}

			template <typename ReturnType, typename DistributionType = Distribution::Binomial<ReturnType>>
			ReturnType random(const double _probability, const ReturnType _min, const ReturnType _max)
			{
				static_assert(std::is_arithmetic<ReturnType>::value, "ReturnType needs to be an arithmetic value!");
				assert(_min <= _max);
				DistributionType dist(double(_max - _min), _probability);
				return _min + dist(engine);
			}

			//Special functions
			glm::vec2 circle(const float _radius = 1.0f)
			{
				float angle = random<float>(0.0f, 2.0f * PI);
				return glm::vec2(cos(angle) * _radius, sin(angle) * _radius);
			}

			glm::vec2 square(const float _side = 1.0f)
			{
				return glm::vec2(random<float>(0.0f, _side), random<float>(0.0f, _side));
			}

			template<typename FloatingPointType = float>
			FloatingPointType unit()
			{
				static_assert(std::is_floating_point<FloatingPointType>::value, "Unit random uses only floating points!");
				return random<FloatingPointType>(static_cast<FloatingPointType>(0.0f), static_cast<FloatingPointType>(1.0f));
			}

			template<typename FloatingPointType = float>
			FloatingPointType angle()
			{
				static_assert(std::is_floating_point<FloatingPointType>::value, "Angle random uses only floating points!");
				return random<FloatingPointType>(static_cast<FloatingPointType>(0.0f), static_cast<FloatingPointType>(2.0f * PI));
			}

			bool coin()
			{
				return random<bool>();
			}

			bool weightedCoin(const double _trueProbability = 0.5)
			{
				return random<bool, Distribution::Bernoulli>(_trueProbability);
			}

			unsigned dice(const unsigned _sides)
			{
				return random<unsigned>(1, _sides);
			}

			unsigned weightedDice(const std::initializer_list<double> _propabilities)
			{
				Distribution::Discrete<unsigned> dist(_propabilities);
				return dist(engine) + 1;
			}

		private:
			SeedType randomSeed;
			MTEngine engine;
		};
		extern PRNG<uint32_t> defaultRandom;
	}
}