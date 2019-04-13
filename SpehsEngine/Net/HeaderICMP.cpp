#include "stdafx.h"
#include "SpehsEngine/Net/HeaderICMP.h"

namespace se
{
	namespace net
	{
		HeaderICMP::HeaderICMP()
		{
			std::fill(rep_, rep_ + sizeof(rep_), 0);
		}

		void HeaderICMP::setType(const uint8_t n)
		{
			rep_[0] = n;
		}

		void HeaderICMP::setCode(const uint8_t n)
		{
			rep_[1] = n;
		}

		void HeaderICMP::setChecksum(const uint16_t n)
		{
			encode(2, 3, n);
		}

		void HeaderICMP::setIdentifier(const uint16_t n)
		{
			encode(4, 5, n);
		}

		void HeaderICMP::setSequenceNumber(const uint16_t n)
		{
			encode(6, 7, n);
		}

		uint8_t HeaderICMP::getType() const
		{
			return rep_[0];
		}

		uint8_t HeaderICMP::getCode() const
		{
			return rep_[1];
		}

		uint16_t HeaderICMP::getChecksum() const
		{
			return decode(2, 3);
		}

		uint16_t HeaderICMP::getIdentifier() const
		{
			return decode(4, 5);
		}

		uint16_t HeaderICMP::getSequenceNumber() const
		{
			return decode(6, 7);
		}
		
		uint16_t HeaderICMP::decode(const int a, const int b) const
		{
			return (rep_[a] << 8) + rep_[b];
		}

		void HeaderICMP::encode(const int a, const int b, const uint16_t n)
		{
			rep_[a] = static_cast<uint8_t>(n >> 8);
			rep_[b] = static_cast<uint8_t>(n & 0xFF);
		}
	}
}
