#pragma once

namespace boost
{
	namespace asio
	{
		class io_context;
	}
}

namespace se
{
	namespace net
	{
		class IOService;
		class ISocket;
		class Connection;
		class ConnectionManager;
		struct ISocketTCP;
		struct NetIdentity;
		struct ConnectionParameters;
		template<typename PacketType> class Packetman;
		enum class DisconnectType : uint8_t;
		enum class PacketType : uint8_t;
	}
}

#include "SpehsEngine/Net/Endpoint.h"
