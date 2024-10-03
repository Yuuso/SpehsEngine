#pragma once


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

		ScopedConnection& add();
		void clear();

	private:
		std::vector<ScopedConnection> vector;
	};
}