#pragma once
#include <stdint.h>

namespace se
{
	class WriteBuffer;
	class ReadBuffer;
	namespace net
	{
		/*
			Data exchanged between spehs sockets upon connecting.
		*/
		class Handshake
		{
		public:

			void write(WriteBuffer& buffer) const;
			bool read(ReadBuffer& buffer);

			bool isValid() const;

		private:
			typedef uint16_t VersionType;
			static const VersionType currentVersion = 1;
			static const uint64_t currentMagic = 0xC0DEC5F070C01001;//Mmmm...magic
			uint64_t magic = currentMagic;
			VersionType handshakeVersion = currentVersion;
			bool valid = true;
		};
	}
}