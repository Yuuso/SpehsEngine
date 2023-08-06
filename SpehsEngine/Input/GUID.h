#pragma once

#include <stdint.h>

namespace se
{
	struct GUID
	{
		GUID() = default;
		GUID(const GUID& other);
		void operator=(const GUID& other);
		void operator=(GUID&& other);
		bool operator==(const GUID& other) const;
		bool operator!=(const GUID& other) const;
		uint8_t operator[](const int index) const;
		uint8_t guid[16];
	};
}
