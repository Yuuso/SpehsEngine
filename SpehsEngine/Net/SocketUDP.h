#pragma once

#include "SpehsEngine/Net/ISocket.h"
#include "SpehsEngine/Net/Endpoint.h"
#include "SpehsEngine/Net/PacketMessage.h"


namespace se
{
	namespace net
	{
		struct ISocketUDP : public ISocket
		{
			typedef uint32_t ExpectedBytesType;
			typedef uint32_t Id;

			static std::unique_ptr<ISocketUDP> create(IOService& ioService);
			virtual ~ISocketUDP() = default;

			/* Opens the socket. Must be called before packets can be received or sent. */
			virtual bool open() = 0;
			virtual void close() = 0;
			/* Binds the socket to a local endpoint. */
			virtual bool bind(const Port& port) = 0;

			/*
				Connects the socket to a remote endpoint. Blocking call.
				Socket must be opened before calling.
				UDP is a connectionless protocol, connection status cannot be maintained.
			*/
			virtual bool connect(const Endpoint& remoteEndpoint) = 0;
			/* Clears the connected endpoint value. */
			virtual void disconnect() = 0;

			/* Process arrived packets(onReceive callbacks). */
			virtual void update() = 0;

			/* Sends buffer to the set remote endpoint. */
			virtual bool sendPacket(const BinaryWriter& binaryWriter, const PacketType packetType = PacketType::undefined) = 0;

			/* Sends buffer to a specified endpoint. */
			virtual bool sendPacket(const BinaryWriter& binaryWriter, const Endpoint& endpoint, const PacketType packetType = PacketType::undefined) = 0;

			/* Returns false if the memory allocation fails, or the socket is currently receiving data. */
			virtual bool resizeReceiveBuffer(const size_t newSize) = 0;

			/* Starts receiving data from the connected endpoint. Non-blocking call. */
			virtual bool startReceiving() = 0;
			virtual bool isReceiving() const = 0;

			/* Heartbeat packets are sent between set intervals (requires calling update()). When set to 0, no heartbeats are sent. */
			virtual void setHeartbeatInterval(const Time interval) = 0;
			virtual Time getHeartbeatInterval() const = 0;
			
			/* Received packets must be processed with a specified receive handler. While there exists no callback, all incoming packets are discarded. */
			virtual void setOnReceiveCallback(const std::function<void(BinaryReader&, const Endpoint&)> onReceiveCallback = std::function<void(BinaryReader&, const Endpoint&)>()) = 0;

			virtual void setReuseAddress(const bool enabled) = 0;
			virtual bool getReuseAddress() const = 0;

			virtual bool isOpen() const = 0;
			virtual Port getLocalPort() const = 0;

			virtual bool isConnected() const = 0;
			virtual Endpoint getLocalEndpoint() const = 0;
			virtual Endpoint getConnectedEndpoint() const = 0;

			virtual void setDebugLogLevel(const int level) = 0;
			virtual int getDebugLogLevel() const = 0;
		};
	}
}
