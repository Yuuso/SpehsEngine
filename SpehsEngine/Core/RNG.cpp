#include "stdafx.h"

#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Core/Log.h"

#include <boost/random/random_device.hpp>

#include <algorithm>
#include <string>
#include <chrono>


namespace se
{
	namespace rng
	{
		PRNG<uint32_t> defaultRandom;
		std::mutex rngmutex;
		void initialize()
		{
			// 'True' random source
			boost::random_device source;
			log::info("RNG: random source entropy: " + std::to_string(source.entropy()));

#if defined (SE_RNG_USE_FULL_RANDOM_SEED)
			const size_t randomDataSize = std::mt19937::state_size;
#else
			const size_t randomDataSize = 2;
#endif
			// 624 byte random seed data for mt19937
			std::mt19937::result_type random_data[randomDataSize];
			std::generate(std::begin(random_data), std::prev(std::end(random_data)), std::ref(source));

			// Since random_device may not be always reliable, we'll use current time from chrono as well
			std::mt19937::result_type timeSeed = (std::mt19937::result_type)std::chrono::system_clock::now().time_since_epoch().count();
			random_data[randomDataSize - 1] = timeSeed;

			std::seed_seq seeds(std::begin(random_data), std::end(random_data));

			rngmutex.lock();
			defaultRandom.seed(seeds);
			rngmutex.unlock();
		}

		float unit()
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return defaultRandom.unit();
		}
		bool coin()
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return defaultRandom.coin();
		}
		bool weightedCoin(const double _trueProbability)
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return defaultRandom.weightedCoin(_trueProbability);
		}
		unsigned dice(const unsigned _sides)
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return defaultRandom.dice(_sides);
		}
		unsigned weightedDice(const std::initializer_list<double> _propabilities)
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return defaultRandom.weightedDice(_propabilities);
		}
		glm::vec2 circle(const float _radius)
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return defaultRandom.circle(_radius);
		}
		glm::vec2 square(const float _side)
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return defaultRandom.square(_side);
		}
		glm::vec2 square(const glm::vec2& _min, const glm::vec2& _max)
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return defaultRandom.square(_min, _max);
		}
		glm::vec3 cube(const float _side)
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return defaultRandom.cube(_side);
		}
		glm::vec3 cube(const glm::vec3& _min, const glm::vec3& _max)
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return defaultRandom.cube(_min, _max);
		}
		glm::quat rotation()
		{
			const float u = unit(), v = unit(), w = unit();
			return glm::quat(sqrtf(1.0f - u) * sinf(glm::two_pi<float>() * v),
							 sqrtf(1.0f - u) * cosf(glm::two_pi<float>() * v),
							 sqrtf(u) * sinf(glm::two_pi<float>() * w),
							 sqrtf(u) * cosf(glm::two_pi<float>() * w));
		}
		glm::vec2 direction2D()
		{
			const float a = angle();
			return glm::vec2(cos(a), sin(a));
		}
		glm::vec3 direction3D()
		{
			return rotation() * glm::vec3(0.0f, 0.0f, 1.0f);
		}
	}
}