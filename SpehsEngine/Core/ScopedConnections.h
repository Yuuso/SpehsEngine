#pragma once

#include <vector>

namespace boost
{
	namespace signals2
	{
		class scoped_connection;
	}
}


namespace se
{
	// Utility class for adding anonymous scoped connections
	class ScopedConnections
	{
	public:

		ScopedConnections() = default;
		ScopedConnections(const ScopedConnections& _copy) = default;
		ScopedConnections(ScopedConnections&& _move) = default;
		~ScopedConnections();

		ScopedConnections& operator=(const ScopedConnections& _copy) = default;
		ScopedConnections& operator=(ScopedConnections&& _move) = default;

		boost::signals2::scoped_connection& add();
		void clear();

	private:
		std::vector<boost::signals2::scoped_connection> vector;
	};
}