#pragma once

#include <random> // TODO: remove


namespace se
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

		//Pseudo random number generator class
		template <typename SeedType>
		class PRNG
		{
			static_assert(std::is_unsigned<SeedType>::value && (sizeof(SeedType) == 4 || sizeof(SeedType) == 8), "SeedType needs to be either 32bit or 64bit unsigned integer type!");
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
				//Maybe needs some clarification...
				engine.seed(_seed);
				randomSeed = 0;
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
			template <
				typename ReturnType,
				typename DistributionType = typename std::conditional<std::is_floating_point<ReturnType>::value, Distribution::UniformReal<ReturnType>, Distribution::UniformInt<ReturnType>>::type>
				typename std::enable_if<sizeof(ReturnType) != 1, ReturnType>::type random()
			{
				return random<ReturnType>(std::numeric_limits<ReturnType>::min(), std::numeric_limits<ReturnType>::max());
			}

			template <typename ReturnType, typename DistributionType = Distribution::UniformInt<int>>
			typename std::enable_if<sizeof(ReturnType) == 1, ReturnType>::type random()
			{
				return (ReturnType)random<ReturnType>(std::numeric_limits<ReturnType>::min(), std::numeric_limits<ReturnType>::max());
			}

			template <
				typename ReturnType,
				typename DistributionType = typename std::conditional<std::is_integral<ReturnType>::value, Distribution::UniformInt<ReturnType>, Distribution::UniformReal<ReturnType>>::type>
				typename std::enable_if<sizeof(ReturnType) != 1, ReturnType>::type random(const ReturnType _min, const ReturnType _max)
			{
				static_assert(std::is_arithmetic<ReturnType>::value, "ReturnType needs to be an arithmetic type!");
				se_assert(_min <= _max);
				DistributionType dist;
				return dist(engine, typename DistributionType::param_type(_min, _max));
			}

			template <typename ReturnType, typename DistributionType = Distribution::UniformInt<int>>
			typename std::enable_if<sizeof(ReturnType) == 1, ReturnType>::type random(const ReturnType _min, const ReturnType _max)
			{
				static_assert(std::is_arithmetic<ReturnType>::value, "ReturnType needs to be an arithmetic type!");
				se_assert(_min <= _max);
				DistributionType dist;
				return dist(engine, DistributionType::param_type(_min, _max));
			}

			template <>
			bool random(const bool _min, const bool _max)
			{
				Distribution::UniformInt<int> dist;
				return dist(engine, Distribution::UniformInt<int>::param_type(_min, _max)) != 0;
			}

			//Functions for certain distirbutions
			template <typename ReturnType, typename DistributionType = Distribution::Bernoulli>
			ReturnType random(const double _probability)
			{
				static_assert(std::is_arithmetic<ReturnType>::value, "ReturnType needs to be an arithmetic type!");
				DistributionType dist(_probability);
				return dist(engine);
			}

			template <typename ReturnType, typename DistributionType = Distribution::Binomial<ReturnType>>
			ReturnType random(const double _probability, const ReturnType _min, const ReturnType _max)
			{
				static_assert(std::is_arithmetic<ReturnType>::value, "ReturnType needs to be an arithmetic type!");
				se_assert(_min <= _max);
				DistributionType dist(double(_max - _min), _probability);
				return _min + dist(engine);
			}

			//Special functions
			glm::vec2 circle(const float _radius = 1.0f)
			{
				const float angle = random<float>(0.0f, static_cast<float>(6.283185307179586476925286766559L));
				return glm::vec2(cos(angle) * _radius, sin(angle) * _radius);
			}

			glm::vec2 square(const float _side = 1.0f)
			{
				return glm::vec2(random<float>(0.0f, _side), random<float>(0.0f, _side));
			}
			glm::vec2 square(const glm::vec2& _min, const glm::vec2& _max)
			{
				return glm::vec2(random<float>(_min.x, _max.x), random<float>(_min.y, _max.y));
			}

			glm::vec3 cube(const float _side = 1.0f)
			{
				return glm::vec3(random<float>(0.0f, _side), random<float>(0.0f, _side), random<float>(0.0f, _side));
			}
			glm::vec3 cube(const glm::vec3& _min, const glm::vec3& _max)
			{
				return glm::vec3(random<float>(_min.x, _max.x), random<float>(_min.y, _max.y), random<float>(_min.z, _max.z));
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
				return random<FloatingPointType>(static_cast<FloatingPointType>(0.0f), static_cast<FloatingPointType>(6.283185307179586476925286766559L));
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

		// General 'true' random functions
		extern std::mutex rngmutex;
		extern PRNG<uint32_t> defaultRandom;

		template <typename ReturnType, typename DistributionType =
			typename std::conditional<std::is_floating_point<ReturnType>::value, Distribution::UniformReal<ReturnType>, Distribution::UniformInt<ReturnType>>::type>
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
			typename std::conditional<std::is_floating_point<ReturnType>::value, Distribution::UniformReal<ReturnType>, Distribution::UniformInt<ReturnType>>::type>
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
		glm::vec2 circle(const float _radius = 1.0f);
		glm::vec2 square(const float _side = 1.0f);
		glm::vec2 square(const glm::vec2& _min, const glm::vec2& _max);
		glm::vec3 cube(const float _side = 1.0f);
		glm::vec3 cube(const glm::vec3& _min, const glm::vec3& _max);
		glm::quat rotation();
		glm::vec2 direction2D();
		glm::vec3 direction3D();
	}
}