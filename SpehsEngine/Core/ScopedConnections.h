#pragma once

#include "boost/signals2/connection.hpp"
#include <vector>


namespace se
{
	// Utility class for adding anonymous scoped connections
	class ScopedConnections
	{
	public:

		boost::signals2::scoped_connection& add()
		{
			vector.push_back(boost::signals2::scoped_connection());
			return vector.back();
		}

	private:
		std::vector<boost::signals2::scoped_connection> vector;
	};
}