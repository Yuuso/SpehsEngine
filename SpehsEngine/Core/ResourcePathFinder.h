#pragma once

#include <string>


namespace se
{
	class ResourcePathFinder
	{
	public:

		// Should return full relative path to the given resource name
		virtual std::string getPath(const std::string_view _resource) const
		{
			return std::string(_resource);
		}
	};
}
