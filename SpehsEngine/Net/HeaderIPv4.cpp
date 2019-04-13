#include "stdafx.h"
#include "SpehsEngine/Net/SocketICMP.h"

namespace se
{
	namespace net
	{
		HeaderIPv4::HeaderIPv4()
		{
			std::fill(rep, rep + sizeof(rep), 0);
		}

		uint8_t HeaderIPv4::getVersion() const
		{
			return (rep[0] >> 4) & 0xF;
		}

		uint16_t HeaderIPv4::getHeaderLength() const
		{
			return (rep[0] & 0xF) * 4;
		}

		uint8_t HeaderIPv4::getTypeOfService() const
		{
			return rep[1];
		}

		uint16_t HeaderIPv4::getTotalLength() const
		{
			return decode(2, 3);
		}

		uint16_t HeaderIPv4::getIdentification() const
		{
			return decode(4, 5);
		}

		bool HeaderIPv4::getDontFragment() const
		{
			return (rep[6] & 0x40) != 0;
		}

		bool HeaderIPv4::getMoreFragments() const
		{
			return (rep[6] & 0x20) != 0;
		}

		uint16_t HeaderIPv4::getFragmentOffset() const
		{
			return decode(6, 7) & 0x1FFF;
		}

		uint32_t HeaderIPv4::getTimeToLive() const
		{
			return rep[8];
		}

		uint8_t HeaderIPv4::getProtocol() const
		{
			return rep[9];
		}

		uint16_t HeaderIPv4::getHeaderChecksum() const
		{
			return decode(10, 11);
		}

		boost::asio::ip::address_v4 HeaderIPv4::getSourceAddress() const
		{
			const boost::asio::ip::address_v4::bytes_type bytes = { { rep[12], rep[13], rep[14], rep[15] } };
			return boost::asio::ip::address_v4(bytes);
		}

		boost::asio::ip::address_v4 HeaderIPv4::getDestinationAddress() const
		{
			const boost::asio::ip::address_v4::bytes_type bytes = { { rep[16], rep[17], rep[18], rep[19] } };
			return boost::asio::ip::address_v4(bytes);
		}

		uint16_t HeaderIPv4::decode(int a, int b) const
		{
			return (rep[a] << 8) + rep[b];
		}
	}
}
