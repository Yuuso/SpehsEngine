#include "stdafx.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"

#include "boost/lexical_cast.hpp"
#include "SpehsEngine/Core/StringOperations.h"


namespace se
{
	bool doesStartWith(const std::string_view string, const std::string_view searchParameter)
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

	bool doesEndWith(const std::string_view string, const std::string_view searchParameter)
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

	bool doesContain(const std::string_view string, const std::string_view searchParameter)
	{
		return string.find(searchParameter) != std::string::npos;
	}
	
	void indent(std::string& string, const std::string_view indentation)
	{
		if (string.empty())
		{
			return;
		}

		string.insert(string.begin(), indentation.begin(), indentation.end());

		for (size_t i = indentation.size(); i < string.size() - 1; i++)
		{
			if (string[i] == '\n')
			{
				string.insert(string.begin() + i + 1, indentation.begin(), indentation.end());
				i += indentation.size();
			}
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
		if (bytes >= 1024ull * 1024ull * 1024ull * 1024ull * 1024ull)
		{
			return se::toString(float(double(bytes) / double(1024ull * 1024ull * 1024ull * 1024ull * 1024ull)), 2) + " PB";
		}
		else if (bytes >= 1024ull * 1024ull * 1024ull * 1024ull)
		{
			return se::toString(float(double(bytes) / double(1024ull * 1024ull * 1024ull * 1024ull)), 2) + " TB";
		}
		else if (bytes >= 1024ull * 1024ull * 1024ull)
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

	template<typename T>
	inline bool fromStringImpl(const std::string_view string, T& valueOut)
	{
		try
		{
			valueOut = boost::lexical_cast<T>(string);
			return true;
		}
		catch (const std::exception&)
		{
			valueOut = 0;
			return false;
		}
	}

	bool fromString(const std::string_view string, int64_t& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	bool fromString(const std::string_view string, int32_t& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	bool fromString(const std::string_view string, int16_t& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	bool fromString(const std::string_view string, int8_t& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	bool fromString(const std::string_view string, uint64_t& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	bool fromString(const std::string_view string, uint32_t& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	bool fromString(const std::string_view string, uint16_t& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	bool fromString(const std::string_view string, uint8_t& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	bool fromString(const std::string_view string, float& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	bool fromString(const std::string_view string, double& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}
}
