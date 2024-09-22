#include "stdafx.h"
#include "SpehsEngine/Core/ScopedConnections.h"


namespace se
{
	// Implementations that require the include
	ScopedConnections::~ScopedConnections()
	{
	}

	boost::signals2::scoped_connection& ScopedConnections::add()
	{
		vector.push_back(boost::signals2::scoped_connection());
		return vector.back();
	}

	void ScopedConnections::clear()
	{
		vector.clear();
	}
}
