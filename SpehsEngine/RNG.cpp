
#include "RNG.h"

#include <boost/random/random_device.hpp>

#include <algorithm>


namespace spehs
{
	namespace rng
	{
		PRNG<uint32_t> defaultRandom;
		void initialize()
		{
			std::mt19937::result_type random_data[std::mt19937::state_size];
			boost::random_device source;
			std::generate(std::begin(random_data), std::end(random_data), std::ref(source));
			std::seed_seq seeds(std::begin(random_data), std::end(random_data));
			defaultRandom.seed(seeds);
		}

		float unit()
		{
			return defaultRandom.unit();
		}
		bool coin()
		{
			return defaultRandom.coin();
		}
		bool weightedCoin(const double _trueProbability)
		{
			return defaultRandom.weightedCoin(_trueProbability);
		}
		unsigned dice(const unsigned _sides)
		{
			return defaultRandom.dice(_sides);
		}
		unsigned weightedDice(const std::initializer_list<double> _propabilities)
		{
			return defaultRandom.weightedDice(_propabilities);
		}
	}
}