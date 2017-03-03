
#include <stdlib.h>
#include <time.h>

#include "RNG.h"
#include "Exceptions.h"


std::mt19937 randomMTEngine;
std::uniform_int_distribution<int> intDist;
std::uniform_int_distribution<unsigned int> uintDist;
std::uniform_real_distribution<float> floatDist;
std::uniform_real_distribution<double> doubleDist;
std::uniform_int_distribution<long long> lintDist;
std::uniform_int_distribution<unsigned long long> luintDist;


namespace spehs
{
	namespace rng
	{
		void initializeRNG()
		{
			std::random_device randomDevice;
			randomMTEngine.seed(randomDevice());
		}


		int irandom()
		{
			return sirandom();
		}
		int irandom(const int _min, const int _max)
		{
			if (_min > _max)
			{
				spehs::exceptions::unexpectedError("Min value is greater than Max value! (irandom)");
				return _min;
			}
			return sirandom(_min, _max);
		}
		unsigned int uirandom()
		{
			return uintDist(randomMTEngine, { 0, UINT_MAX });
		}
		unsigned int uirandom(const unsigned int _min, const unsigned int _max)
		{
			if (_min > _max)
			{
				spehs::exceptions::unexpectedError("Min value is greater than max value! (uirandom)");
				return _min;
			}
			if (_min == _max)
				return _min;
			else
				return uintDist(randomMTEngine, { _min, _max });
		}
		unsigned int uirandom(const unsigned int _min, const unsigned int _max, const unsigned int _maxmin, const unsigned int _minmax)
		{
			if (_maxmin >= _minmax)
			{
				spehs::exceptions::unexpectedError("Maxmin value is greater than minmax value! (uirandom)");
				return _min;
			}
			if (_maxmin < _min || _minmax > _max)
			{
				spehs::exceptions::unexpectedError("Ranges of min-max stuff is wrong!! (uirandom)");
				return _min;
			}

			if (sirandom(0, 1))
			{
				return uintDist(randomMTEngine, { _min, _maxmin });
			}
			else
			{
				return uintDist(randomMTEngine, { _minmax, _max });
			}
		}
		int sirandom()
		{
			return intDist(randomMTEngine, { -INT_MAX, INT_MAX });
		}
		int sirandom(int _min, int _max)
		{
			if (_min > _max)
			{
				spehs::exceptions::unexpectedError("Min value is greater than max value! (sirandom)");
				return _min;
			}
			if (_min == _max)
				return _min;
			else
				return intDist(randomMTEngine, { _min, _max });
		}
		int sirandom(int _min, int _max, int _maxmin, int _minmax)
		{
			if (_maxmin >= _minmax)
			{
				spehs::exceptions::unexpectedError("Maxmin value is greater than minmax value! (sirandom)");
				return _min;
			}
			if (_maxmin < _min || _minmax > _max)
			{
				spehs::exceptions::unexpectedError("Ranges of min-max stuff is wrong!! (sirandom)");
				return _min;
			}

			if (sirandom(0, 1))
			{
				return intDist(randomMTEngine, { _min, _maxmin });
			}
			else
			{
				return intDist(randomMTEngine, { _minmax, _max });
			}
		}
		float frandom()
		{
			return floatDist(randomMTEngine, { -FLT_MAX, FLT_MAX });
		}
		float frandom(const float _min, const float _max)
		{
			if (_min > _max)
			{
				spehs::exceptions::unexpectedError("Min value is greater than max value! (frandom)");
				return _min;
			}
			return floatDist(randomMTEngine, { _min, _max });
		}
		float frandom(const float _min, const float _max, const float _maxmin, const float _minmax)
		{
			if (_maxmin >= _minmax)
			{
				spehs::exceptions::unexpectedError("Maxmin value is greater than minmax value! (frandom)");
				return _min;
			}
			if (_maxmin < _min || _minmax > _max)
			{
				spehs::exceptions::unexpectedError("Ranges of min-max stuff is wrong!! (frandom)");
				return _min;
			}

			if (sirandom(0, 1))
			{
				return floatDist(randomMTEngine, { _min, _maxmin });
			}
			else
			{
				return floatDist(randomMTEngine, { _minmax, _max });
			}
		}
		double drandom()
		{
			return doubleDist(randomMTEngine, { -DBL_MAX, DBL_MAX });
		}
		double drandom(const double& _min, const double& _max)
		{
			if (_min > _max)
			{
				spehs::exceptions::unexpectedError("Min value is greater than max value! (drandom)");
				return _min;
			}
			return doubleDist(randomMTEngine, { _min, _max });
		}
		double drandom(const double& _min, const double& _max, const double& _maxmin, const double& _minmax)
		{
			if (_maxmin >= _minmax)
			{
				spehs::exceptions::unexpectedError("Maxmin value is greater than minmax value! (drandom)");
				return _min;
			}
			if (_maxmin < _min || _minmax > _max)
			{
				spehs::exceptions::unexpectedError("Ranges of min-max stuff is wrong!! (drandom)");
				return _min;
			}

			if (sirandom(0, 1))
			{
				return doubleDist(randomMTEngine, { _min, _maxmin });
			}
			else
			{
				return doubleDist(randomMTEngine, { _minmax, _max });
			}
		}
		bool coin()
		{
			return uintDist(randomMTEngine, { 0, 1 });
		}


		PRNG32::PRNG32()
		{
			MTEngine = std::mt19937();
			seed = MTEngine.default_seed;
		}
		PRNG32::PRNG32(const unsigned int _initSeed)
		{
			seed = _initSeed;
			MTEngine = std::mt19937(seed);
		}
		PRNG32::~PRNG32()
		{

		}
		void PRNG32::setSeed(const unsigned int _newSeed)
		{
			seed = _newSeed;
			MTEngine.seed(seed);
		}
		unsigned int PRNG32::getSeed() const
		{
			return seed;
		}
		void PRNG32::reset()
		{
			MTEngine.seed(seed);
		}
		uint_fast32_t PRNG32::uirandom()
		{
			return MTEngine();
		}
		uint_fast32_t PRNG32::uirandom(const unsigned int _min, const unsigned int _max)
		{
			if (_min > _max)
			{
				spehs::exceptions::unexpectedError("Min value is greater than Max value! (PRNG32.sirandom)");
				return _min;
			}
			if (_min == _max)
				return _min;
			else
				return uintDist(MTEngine, { _min, _max });
		}
		uint_fast32_t PRNG32::uirandom(const unsigned int _min, const unsigned int _max, const unsigned int _maxmin, const unsigned int _minmax)
		{
			if (_maxmin >= _minmax)
			{
				spehs::exceptions::unexpectedError("Maxmin value is greater than minmax value! (PRNG32.uirandom)");
				return _min;
			}
			if (_maxmin < _min || _minmax > _max)
			{
				spehs::exceptions::unexpectedError("Ranges of min-max stuff is wrong!! (PRNG32.uirandom)");
				return _min;
			}

			if (rng::sirandom(0, 1))
			{
				return uintDist(MTEngine, { _min, _maxmin });
			}
			else
			{
				return uintDist(MTEngine, { _minmax, _max });
			}
		}
		int_fast32_t PRNG32::sirandom()
		{
			return intDist(MTEngine, { -INT_MAX, INT_MAX });
		}
		int_fast32_t PRNG32::sirandom(const int _min, const int _max)
		{
			if (_min > _max)
			{
				spehs::exceptions::unexpectedError("Min value is greater than Max value! (PRNG32.sirandom)");
				return _min;
			}
			if (_min == _max)
				return _min;
			else
				return intDist(MTEngine, { _min, _max });
		}
		int_fast32_t PRNG32::sirandom(const int _min, const int _max, const int _maxmin, const int _minmax)
		{
			if (_maxmin >= _minmax)
			{
				spehs::exceptions::unexpectedError("Maxmin value is greater than minmax value! (PRNG32.sirandom)");
				return _min;
			}
			if (_maxmin < _min || _minmax > _max)
			{
				spehs::exceptions::unexpectedError("Ranges of min-max stuff is wrong!! (PRNG32.sirandom)");
				return _min;
			}

			if (rng::sirandom(0, 1))
			{
				return intDist(MTEngine, { _min, _maxmin });
			}
			else
			{
				return intDist(MTEngine, { _minmax, _max });
			}
		}
		float PRNG32::frandom()
		{
			return floatDist(MTEngine, { -FLT_MAX, FLT_MAX });
		}
		float PRNG32::frandom(const float _min, const float _max)
		{
			if (_min > _max)
			{
				spehs::exceptions::unexpectedError("Min value is greater than Max value! (PRNG32.frandom)");
				return _min;
			}
			if (_min == _max)
				return _min;
			else
				return floatDist(MTEngine, { _min, _max });
		}
		float PRNG32::frandom(const float _min, const float _max, const float _maxmin, const float _minmax)
		{
			if (_maxmin >= _minmax)
			{
				spehs::exceptions::unexpectedError("Maxmin value is greater than minmax value! (PRNG32.frandom)");
				return _min;
			}
			if (_maxmin < _min || _minmax > _max)
			{
				spehs::exceptions::unexpectedError("Ranges of min-max stuff is wrong!! (PRNG32.frandom)");
				return _min;
			}

			if (rng::sirandom(0, 1))
			{
				return floatDist(MTEngine, { _min, _maxmin });
			}
			else
			{
				return floatDist(MTEngine, { _minmax, _max });
			}
		}
		double PRNG32::drandom()
		{
			return doubleDist(MTEngine, { -DBL_MAX, DBL_MAX });
		}
		double PRNG32::drandom(const double& _min, const double& _max)
		{
			if (_min > _max)
			{
				spehs::exceptions::unexpectedError("Min value is greater than Max value! (PRNG32.drandom)");
				return _min;
			}
			if (_min == _max)
				return _min;
			else
				return doubleDist(MTEngine, { _min, _max });
		}
		double PRNG32::drandom(const double& _min, const double& _max, const double& _maxmin, const double& _minmax)
		{
			if (_maxmin >= _minmax)
			{
				spehs::exceptions::unexpectedError("Maxmin value is greater than minmax value! (PRNG32.drandom)");
				return _min;
			}
			if (_maxmin < _min || _minmax > _max)
			{
				spehs::exceptions::unexpectedError("Ranges of min-max stuff is wrong!! (PRNG32.drandom)");
				return _min;
			}

			if (rng::sirandom(0, 1))
			{
				return doubleDist(MTEngine, { _min, _maxmin });
			}
			else
			{
				return doubleDist(MTEngine, { _minmax, _max });
			}
		}
		void PRNG32::discardNext(const int _amount)
		{
			MTEngine.discard(_amount);
		}
		uint_fast32_t PRNG32::getMin() const
		{
			return MTEngine.min();
		}
		uint_fast32_t PRNG32::getMax() const
		{
			return MTEngine.max();
		}


		PRNG64::PRNG64()
		{
			MTEngine = std::mt19937_64();
			seed = MTEngine.default_seed;
		}
		PRNG64::PRNG64(const unsigned long long& _seed)
		{
			seed = _seed;
			MTEngine = std::mt19937_64(seed);
		}
		PRNG64::~PRNG64()
		{

		}
		void PRNG64::setSeed(const unsigned long long& _newSeed)
		{
			seed = _newSeed;
			MTEngine.seed(seed);
		}
		unsigned long long PRNG64::getSeed() const
		{
			return seed;
		}
		void PRNG64::reset()
		{
			MTEngine.seed(seed);
		}
		uint_fast64_t PRNG64::uirandom()
		{
			return MTEngine();
		}
		uint_fast64_t PRNG64::uirandom(const unsigned long long& _min, const unsigned long long& _max)
		{
			if (_min > _max)
			{
				spehs::exceptions::unexpectedError("Min value is greater than Max value! (PRNG64.uirandom)");
				return _min;
			}
			if (_min == _max)
				return _min;
			else
				return luintDist(MTEngine, { _min, _max });
		}
		uint_fast64_t PRNG64::uirandom(const unsigned long long& _min, const unsigned long long& _max, const unsigned long long& _maxmin, const unsigned long long& _minmax)
		{
			if (_maxmin >= _minmax)
			{
				spehs::exceptions::unexpectedError("Maxmin value is greater than minmax value! (PRNG64.uirandom)");
				return _min;
			}
			if (_maxmin < _min || _minmax > _max)
			{
				spehs::exceptions::unexpectedError("Ranges of min-max stuff is wrong!! (PRNG64.uirandom)");
				return _min;
			}

			if (rng::sirandom(0, 1))
			{
				return luintDist(MTEngine, { _min, _maxmin });
			}
			else
			{
				return luintDist(MTEngine, { _minmax, _max });
			}
		}
		int_fast64_t PRNG64::sirandom()
		{
			return lintDist(MTEngine, { -LLONG_MAX, LLONG_MAX });
		}
		int_fast64_t PRNG64::sirandom(const long long& _min, const long long& _max)
		{
			if (_min > _max)
			{
				spehs::exceptions::unexpectedError("Min value is greater than Max value! (PRNG64.sirandom)");
				return _min;
			}
			if (_min == _max)
				return _min;
			else
				return lintDist(MTEngine, { _min, _max });
		}
		int_fast64_t PRNG64::sirandom(const long long& _min, const long long& _max, const long long& _maxmin, const long long& _minmax)
		{
			if (_maxmin >= _minmax)
			{
				spehs::exceptions::unexpectedError("Maxmin value is greater than minmax value! (PRNG64.sirandom)");
				return _min;
			}
			if (_maxmin < _min || _minmax > _max)
			{
				spehs::exceptions::unexpectedError("Ranges of min-max stuff is wrong!! (PRNG64.sirandom)");
				return _min;
			}

			if (rng::sirandom(0, 1))
			{
				return lintDist(MTEngine, { _min, _maxmin });
			}
			else
			{
				return lintDist(MTEngine, { _minmax, _max });
			}
		}
		float PRNG64::frandom()
		{
			return floatDist(MTEngine, { -FLT_MAX, FLT_MAX });
		}
		float PRNG64::frandom(const float _min, const float _max)
		{
			if (_min > _max)
			{
				spehs::exceptions::unexpectedError("Min value is greater than Max value! (PRNG32.frandom)");
				return _min;
			}
			if (_min == _max)
				return _min;
			else
				return floatDist(MTEngine, { _min, _max });
		}
		float PRNG64::frandom(const float _min, const float _max, const float _maxmin, const float _minmax)
		{
			if (_maxmin >= _minmax)
			{
				spehs::exceptions::unexpectedError("Maxmin value is greater than minmax value! (PRNG32.frandom)");
				return _min;
			}
			if (_maxmin < _min || _minmax > _max)
			{
				spehs::exceptions::unexpectedError("Ranges of min-max stuff is wrong!! (PRNG32.frandom)");
				return _min;
			}

			if (rng::sirandom(0, 1))
			{
				return floatDist(MTEngine, { _min, _maxmin });
			}
			else
			{
				return floatDist(MTEngine, { _minmax, _max });
			}
		}
		double PRNG64::drandom()
		{
			return doubleDist(MTEngine, { -DBL_MAX, DBL_MAX });
		}
		double PRNG64::drandom(const double& _min, const double& _max)
		{
			if (_min > _max)
			{
				spehs::exceptions::unexpectedError("Min value is greater than Max value! (PRNG32.drandom)");
				return _min;
			}
			if (_min == _max)
				return _min;
			else
				return doubleDist(MTEngine, { _min, _max });
		}
		double PRNG64::drandom(const double& _min, const double& _max, const double& _maxmin, const double& _minmax)
		{
			if (_maxmin >= _minmax)
			{
				spehs::exceptions::unexpectedError("Maxmin value is greater than minmax value! (PRNG32.drandom)");
				return _min;
			}
			if (_maxmin < _min || _minmax > _max)
			{
				spehs::exceptions::unexpectedError("Ranges of min-max stuff is wrong!! (PRNG32.drandom)");
				return _min;
			}

			if (rng::sirandom(0, 1))
			{
				return doubleDist(MTEngine, { _min, _maxmin });
			}
			else
			{
				return doubleDist(MTEngine, { _minmax, _max });
			}
		}
		void PRNG64::discardNext(const int _amount)
		{
			MTEngine.discard(_amount);
		}
		uint_fast64_t PRNG64::getMin() const
		{
			return MTEngine.min();
		}
		uint_fast64_t PRNG64::getMax() const
		{
			return MTEngine.max();
		}
	}
}