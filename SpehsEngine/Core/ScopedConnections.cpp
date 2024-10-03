#include "stdafx.h"
#include "SpehsEngine/Core/ScopedConnections.h"


namespace se
{
	// Implementations that require the include
	ScopedConnections::~ScopedConnections()
	{
	}

	ScopedConnection& ScopedConnections::add()
	{
		vector.push_back(ScopedConnection());
		return vector.back();
	}

	void ScopedConnections::clear()
	{
		vector.clear();
	}
}
