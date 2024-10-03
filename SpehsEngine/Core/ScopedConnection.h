#pragma once

namespace boost
{
	namespace signals2
	{
		class connection;
		class scoped_connection;
	}
}


namespace se
{
	// Wrapper class for boost::signals2::scoped_connection, because the header is very heavy to be included everywhere.
	class ScopedConnection
	{
	public:

		ScopedConnection();
		ScopedConnection(const ScopedConnection& _copy) = delete;
		ScopedConnection(ScopedConnection&& _move);
		ScopedConnection(boost::signals2::connection&& _move);
		ScopedConnection(boost::signals2::scoped_connection&& _move);
		~ScopedConnection();

		ScopedConnection& operator=(const ScopedConnection& _copy) = delete;
		ScopedConnection& operator=(ScopedConnection&& _move);
		ScopedConnection& operator=(boost::signals2::connection&& _move);
		ScopedConnection& operator=(boost::signals2::scoped_connection&& _move);

		void swap(ScopedConnection& other);
		void disconnect();
		bool connected() const;

		boost::signals2::scoped_connection& get();
		const boost::signals2::scoped_connection& get() const;

	private:
		unsigned char data[16];
	};
}