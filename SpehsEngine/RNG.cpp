
#include "RNG.h"

#include <boost/random/random_device.hpp>

#include <algorithm>


namespace spehs
{
	namespace rng
	{
		PRNG<uint32_t> defaultRandom;
		std::mutex rngmutex;
		void initialize()
		{
			rngmutex.lock();
			std::mt19937::result_type random_data[std::mt19937::state_size];
			boost::random_device source;
			std::generate(std::begin(random_data), std::end(random_data), std::ref(source));
			std::seed_seq seeds(std::begin(random_data), std::end(random_data));
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
		spehs::vec2 circle(const float _radius)
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return defaultRandom.circle(_radius);
		}
		spehs::vec2 square(const float _side)
		{
			std::lock_guard<std::mutex> rnglock(rngmutex);
			return defaultRandom.square(_side);
		}
	}
}