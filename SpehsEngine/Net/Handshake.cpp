#include "Handshake.h"

#include <SpehsEngine/Core/WriteBuffer.h>
#include <SpehsEngine/Core/ReadBuffer.h>
#include <SpehsEngine/Core/Log.h>
#include <SpehsEngine/Core/StringOperations.h>

namespace spehs
{
	namespace net
	{
		bool Handshake::isValid() const
		{
			return valid;
		}

		const uint16_t endiannessCheckBytes = 0xACDC;
		void Handshake::write(WriteBuffer& buffer) const
		{
			buffer.write(currentMagic);
			buffer.write(handshakeVersion);
			buffer.write(endiannessCheckBytes);
		}

		void Handshake::read(ReadBuffer& buffer)
		{//NOTE: buffer can contain invalid data! If so, set the valid boolean to false
			size_t offset = 0;
			valid = true;

			//Magic
			if (buffer.getBytesRemaining() < sizeof(magic))
			{
				spehs::log::info("Handshake::read() invalid handshake. No bytes left to read magic.");
				valid = false;
				return;
			}
			else
				buffer.read(magic);
			if (magic != currentMagic)
			{
				spehs::log::info("Handshake::read() invalid handshake. Incompatible magic - my version: " + spehs::toHexString(currentMagic) + ", read magic: " + spehs::toHexString(magic));
				valid = false;
				return;
			}

			//Handshake version
			if (buffer.getBytesRemaining() < sizeof(handshakeVersion))
			{
				spehs::log::info("Handshake::read() invalid handshake. No bytes left to read handshake version.");
				valid = false;
				return;
			}
			else
				buffer.read((uint32_t&)handshakeVersion);
			if (handshakeVersion != currentVersion)
			{
				spehs::log::info("Handshake::read() invalid handshake. Incompatible versions - my version: " + std::to_string(currentVersion) + ", other version: " + std::to_string(handshakeVersion));
				valid = false;
				return;
			}
			//Endianness check bytes
			uint16_t readEndiannessCheckBytes;
			if (buffer.getBytesRemaining() < sizeof(endiannessCheckBytes))
			{
				spehs::log::info("Handshake::read() invalid handshake. No bytes left to read endianness check bytes.");
				valid = false;
				return;
			}
			else
				buffer.read(readEndiannessCheckBytes);
			if (readEndiannessCheckBytes != endiannessCheckBytes)
			{
				spehs::log::info("Handshake::read() invalid handshake. Invalid endianness check bytes.");
				valid = false;
				return;
			}
		}
	}
}