#pragma once
#include <istream>
#include <ostream>
#include <algorithm>
#include <cstdint>

namespace se
{
	namespace net
	{
		class HeaderICMP
		{
		public:

			enum
			{
				echo_reply = 0, destination_unreachable = 3, source_quench = 4,
				redirect = 5, echo_request = 8, time_exceeded = 11, parameter_problem = 12,
				timestamp_request = 13, timestamp_reply = 14, info_request = 15,
				info_reply = 16, address_request = 17, address_reply = 18
			};

			HeaderICMP();

			void setType(const uint8_t n);
			void setCode(const uint8_t n);
			void setChecksum(const uint16_t n);
			void setIdentifier(const uint16_t n);
			void setSequenceNumber(const uint16_t n);

			uint8_t getType() const;
			uint8_t getCode() const;
			uint16_t getChecksum() const;
			uint16_t getIdentifier() const;
			uint16_t getSequenceNumber() const;

			friend std::istream& operator>>(std::istream& is, HeaderICMP& header)
			{
				return is.read(reinterpret_cast<char*>(header.rep_), 8);
			}

			friend std::ostream& operator<<(std::ostream& os, const HeaderICMP& header)
			{
				return os.write(reinterpret_cast<const char*>(header.rep_), 8);
			}

		private:

			uint16_t decode(const int a, const int b) const;
			void encode(const int a, const int b, const uint16_t n);

			// The wire format of an ICMP header is:
			// 0               8               16                             31
			// +---------------+---------------+------------------------------+      ---
			// |               |               |                              |       ^
			// |     type      |     code      |          checksum            |       |
			// |               |               |                              |       |
			// +---------------+---------------+------------------------------+    8 bytes
			// |                               |                              |       |
			// |          identifier           |       sequence number        |       |
			// |                               |                              |       v
			// +-------------------------------+------------------------------+      ---
			uint8_t rep_[8];
		};

		template <typename Iterator>
		void computeChecksum(HeaderICMP& header, Iterator begin, Iterator end)
		{
			unsigned int sum = (header.getType() << 8) + header.getCode() + header.getIdentifier() + header.getSequenceNumber();

			Iterator it = begin;
			while (it != end)
			{
				sum += (static_cast<uint8_t>(*it++) << 8);
				if (it != end)
				{
					sum += static_cast<uint8_t>(*it++);
				}
			}

			sum = (sum >> 16) + (sum & 0xFFFF);
			sum += (sum >> 16);
			header.setChecksum(static_cast<unsigned short>(~sum));
		}
	}
}
