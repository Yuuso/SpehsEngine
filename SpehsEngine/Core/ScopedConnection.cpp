#include "stdafx.h"
#include "SpehsEngine/Core/ScopedConnection.h"

#include "boost/signals2/connection.hpp"


namespace se
{
	static_assert(sizeof(boost::signals2::scoped_connection) == 16);

	ScopedConnection::ScopedConnection()
	{
		new ((boost::signals2::scoped_connection*)data) boost::signals2::scoped_connection();
	}

	ScopedConnection::ScopedConnection(ScopedConnection&& _move)
	{
		new ((boost::signals2::scoped_connection*)data) boost::signals2::scoped_connection(std::move(_move.get()));
	}

	ScopedConnection::ScopedConnection(boost::signals2::connection&& _move)
	{
		new ((boost::signals2::scoped_connection*)data) boost::signals2::scoped_connection(std::move(_move));
	}

	ScopedConnection::ScopedConnection(boost::signals2::scoped_connection&& _move)
	{
		new ((boost::signals2::scoped_connection*)data) boost::signals2::scoped_connection(std::move(_move));
	}

	ScopedConnection::~ScopedConnection()
	{
		boost::signals2::scoped_connection& impl = get();
		impl.~scoped_connection();
	}

	ScopedConnection& ScopedConnection::operator=(ScopedConnection&& _move)
	{
		get() = std::move(_move.get());
		return *this;
	}

	ScopedConnection& ScopedConnection::operator=(boost::signals2::connection&& _move)
	{
		get() = std::move(_move);
		return *this;
	}

	ScopedConnection& ScopedConnection::operator=(boost::signals2::scoped_connection&& _move)
	{
		get() = std::move(_move);
		return *this;
	}

	void ScopedConnection::swap(ScopedConnection& other)
	{
		get().swap(other.get());
	}

	void ScopedConnection::disconnect()
	{
		get().disconnect();
	}

	bool ScopedConnection::connected() const
	{
		return get().connected();
	}

	boost::signals2::scoped_connection& ScopedConnection::get()
	{
		return *((boost::signals2::scoped_connection*)(data));
	}

	const boost::signals2::scoped_connection& ScopedConnection::get() const
	{
		return *((boost::signals2::scoped_connection*)(data));
	}
}
