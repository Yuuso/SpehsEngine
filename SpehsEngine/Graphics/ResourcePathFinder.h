#pragma once

#include <string>


namespace se
{
	namespace graphics
	{
		class ResourcePathFinder
		{
		public:

			// Should full relative path to the given resource
			virtual std::string getPath(const std::string_view _resource) const
			{
				return std::string(_resource);
			}
		};
	}
}
