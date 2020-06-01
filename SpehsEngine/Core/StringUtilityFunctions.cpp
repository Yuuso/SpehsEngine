#include "stdafx.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"

#include "SpehsEngine/Core/StringOperations.h"


namespace se
{
	bool doesStartWith(const std::string& string, const std::string& searchParameter)
	{
		if (searchParameter.size() > string.size())
		{
			return false;
		}
		else
		{
			for (size_t i = 0; i < searchParameter.size(); i++)
			{
				if (string[i] != searchParameter[i])
				{
					return false;
				}
			}
			return true;
		}
	}

	bool doesEndWith(const std::string& string, const std::string& searchParameter)
	{
		if (searchParameter.size() > string.size())
		{
			return false;
		}
		else
		{
			const size_t offset = string.size() - searchParameter.size();
			for (size_t i = 0; i < searchParameter.size(); i++)
			{
				if (string[offset + i] != searchParameter[i])
				{
					return false;
				}
			}
			return true;
		}
	}

	std::string toTimeLengthString(const time::Time& time, const size_t precision)
	{
		// TODO: proper precision
		if (time.value >= time::conversionRate::second)
		{
			const float f = time.asSeconds();
			return toString(f, precision) + " s";
		}
		else if (time.value >= time::conversionRate::millisecond)
		{
			const float f = time.asMilliseconds();
			return toString(f, precision) + " ms";
		}
		else if (time.value >= time::conversionRate::microsecond)
		{
			const float f = time.asMicroseconds();
			return toString(f, precision) + " us";
		}
		else
		{
			const float f = time.asNanoseconds();
			return toString(f, precision) + " ns";
		}
	}

	std::string toByteString(const uint64_t bytes)
	{
		if (bytes >= 1024ull * 1024ull * 1024ull)
		{
			return se::toString(float(double(bytes) / double(1024ull * 1024ull * 1024ull)), 2) + " GB";
		}
		else if (bytes >= 1024ull * 1024ull)
		{
			return se::toString(float(double(bytes) / double(1024ull * 1024ull)), 2) + " MB";
		}
		else if (bytes >= 1024ull)
		{
			return se::toString(float(double(bytes) / double(1024ull)), 2) + " KB";
		}
		else
		{
			return std::to_string(bytes) + " B";
		}
	}
}
