#pragma once
#include <cstdint>
#include <algorithm>
#include <boost/asio/ip/address_v4.hpp>

namespace se
{
	namespace net
	{
		class HeaderIPv4
		{
		public:
			HeaderIPv4();

			uint8_t getVersion() const;
			uint16_t getHeaderLength() const;
			uint8_t getTypeOfService() const;
			uint16_t getTotalLength() const;
			uint16_t getIdentification() const;
			bool getDontFragment() const;
			bool getMoreFragments() const;
			uint16_t getFragmentOffset() const;
			uint32_t getTimeToLive() const;
			uint8_t getProtocol() const;
			uint16_t getHeaderChecksum() const;
			boost::asio::ip::address_v4 getSourceAddress() const;
			boost::asio::ip::address_v4 getDestinationAddress() const;

			friend std::istream& operator>>(std::istream& is, HeaderIPv4& header)
			{
				is.read(reinterpret_cast<char*>(header.rep), 20);
				if (header.getVersion() != 4)
				{
					is.setstate(std::ios::failbit);
				}
				std::streamsize optionsLength = header.getHeaderLength() - 20;
				if (optionsLength < 0 || optionsLength > 40)
				{
					is.setstate(std::ios::failbit);
				}
				else
				{
					is.read(reinterpret_cast<char*>(header.rep) + 20, optionsLength);
				}
				return is;
			}

		private:
			uint16_t decode(int a, int b) const;

			// The wire format of an IPv4 header is:
			// 
			// 0               8               16                             31
			// +-------+-------+---------------+------------------------------+      ---
			// |       |       |               |                              |       ^
			// |version|header |    type of    |    total length in bytes     |       |
			// |  (4)  | length|    service    |                              |       |
			// +-------+-------+---------------+-+-+-+------------------------+       |
			// |                               | | | |                        |       |
			// |        identification         |0|D|M|    fragment offset     |       |
			// |                               | |F|F|                        |       |
			// +---------------+---------------+-+-+-+------------------------+       |
			// |               |               |                              |       |
			// | time to live  |   protocol    |       header checksum        |   20 bytes
			// |               |               |                              |       |
			// +---------------+---------------+------------------------------+       |
			// |                                                              |       |
			// |                      source IPv4 address                     |       |
			// |                                                              |       |
			// +--------------------------------------------------------------+       |
			// |                                                              |       |
			// |                   destination IPv4 address                   |       |
			// |                                                              |       v
			// +--------------------------------------------------------------+      ---
			// |                                                              |       ^
			// |                                                              |       |
			// /                        options (if any)                      /    0 - 40
			// /                                                              /     bytes
			// |                                                              |       |
			// |                                                              |       v
			// +--------------------------------------------------------------+      ---
			uint8_t rep[60];
		};
	}
}
