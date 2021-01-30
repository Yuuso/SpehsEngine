#pragma once

#include "SpehsEngine/Core/Archive.h"


namespace se
{
	bool writeToFile(const std::string_view filepath, const se::Archive& archive);
	bool readFromFile(const std::string_view filepath, se::Archive& archive);

	template<typename T>
	typename std::enable_if<!std::is_same<T, Archive>::value, bool>::type writeToFile(const std::string_view filepath, const T& t)
	{
		if (filepath.empty())
		{
			return false;
		}

		const se::Archive archive = t.write();
		return writeToFile(filepath, archive);
	}

	template<typename T>
	typename std::enable_if<!std::is_same<T, Archive>::value, bool>::type readFromFile(const std::string_view filepath, T& t)
	{
		se::Archive archive;
		if (!readFromFile(filepath, archive))
		{
			return false;
		}
		return t.read(archive);
	}
}
