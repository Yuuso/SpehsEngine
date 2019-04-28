#include "stdafx.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"

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
}
