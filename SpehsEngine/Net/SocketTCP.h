#pragma once

#include "SpehsEngine/Net/ISocket.h"
#include "SpehsEngine/Net/Endpoint.h"
#include "SpehsEngine/Net/PacketMessage.h"


namespace se
{
	namespace net
	{
		struct ISocketTCP : public ISocket
		{
			typedef uint32_t ExpectedBytesType;
			typedef uint32_t Id;

			enum class AcceptingState { idle, listeningForConnection, establishingConnection };

			static std::unique_ptr<ISocketTCP> create(IOService& ioService);

			virtual ~ISocketTCP() = default;

			/* Opens the socket with an available port. */
			virtual bool open() = 0;
			virtual void close() = 0;
			virtual bool bind(const Port& port) = 0;

			/*
				Process incoming connections(onAccept callbacks).
				Process arrived packets(onReceive callbacks).
			*/
			virtual void update() = 0;

			/* Perform a synchronous connection attempt. */
			virtual bool connect(const Endpoint& endpoint) = 0;

			/* Disconnects the current connection. Socket still remains open, bound to its port. */
			virtual void disconnect() = 0;

			/* Sends buffer to the connected endpoint. Spehs-level packet type specification is also possible (only for advanced use). */
			virtual bool sendPacket(const BinaryWriter& BinaryWriter, const PacketType packetType = PacketType::undefined) = 0;

			/* Try placing a packet into the receive queue to be processed as if it was received through the connection. Returns false if not connected. */
			virtual bool placeDataOnReceiveQueue(BinaryWriter& binaryWriter) = 0;

			/* Returns false if the memory allocation fails, or the socket is currently receiving data. */
			virtual bool resizeReceiveBuffer(const size_t newSize) = 0;

			/* Socket keeps receiving packets as long as connection is alive. Received packets must be processed with a specified receive handler. */
			virtual void setOnReceiveCallback(const std::function<void(BinaryReader&)> onReceiveCallback = std::function<void(BinaryReader&)>()) = 0;
			virtual bool hasOnReceiveCallback() const = 0;

			/* Starts listening for a new incoming connection. Upon success, a connection is made. Non-blocking call. Callback is called even if no connection is made! */
			virtual bool startAccepting(const Port& port, const std::function<void(ISocketTCP&)> callback) = 0;
			
			/* Stops accepting an incoming connection. */
			virtual void stopAccepting() = 0;

			/* Enabling the 'no delay' option disables Nagle's algorithm. */
			virtual void setNoDelay(const bool enabled) = 0;
			virtual bool getNoDelay() const = 0;

			virtual void setReuseAddress(const bool enabled) = 0;
			virtual bool getReuseAddress() const = 0;

			/* Enabling the 'keep alive' option causes socket to send keep-alive messages. */
			virtual void setKeepAlive(const bool enabled) = 0;
			virtual bool getKeepAlive() const = 0;

			/* Returns the total number of sent bytes. Does not account bytes from the IP and TCP implementation. */
			//virtual size_t getSentBytes() const override = 0;

			/* Returns the total number of received bytes. Does not account bytes from the IP and TCP implementation. */
			//virtual size_t getReceivedBytes() const override = 0;

			/* Remote endpoint. */
			virtual Address getRemoteAddress() const = 0;
			virtual Port getRemotePort() const = 0;
			virtual Endpoint getRemoteEndpoint() const = 0;

			virtual bool isOpen() const = 0;
			virtual Port getLocalPort() const = 0;
			virtual Endpoint getLocalEndpoint() const = 0;

			virtual AcceptingState getAcceptingState() const = 0;
			/* Returns true if the socket is currently listening for an incoming connection on a port, or the accepted connection is currently being established. */
			virtual bool isAccepting() const = 0;
			/* Returns true if socket is currently able to receive incoming packets. */
			virtual bool isReceiving() const = 0;
			/* Returns true if the socket is currently connected to a remote spehs socket. */
			virtual bool isConnected() const = 0;

			virtual void setDebugLogLevel(const int level) = 0;
			virtual int getDebugLogLevel() const = 0;
		};
	}
}
