#pragma once


namespace se
{
	namespace file
	{
		struct Timestamp
		{
			Timestamp() = default;
			Timestamp(const int64_t _value) : value(_value) {}

			int64_t value = 0ull; // Value represents seconds
		};
	}
}