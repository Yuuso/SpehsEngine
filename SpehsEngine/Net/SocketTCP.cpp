#include "stdafx.h"
#include "SpehsEngine/Net/SocketTCP.h"

#include "SpehsEngine/Net/Acceptor.h"
#include "SpehsEngine/Net/IOService.h"
#include "SpehsEngine/Net/IOServiceUtilityFunctions.h"
#include "SpehsEngine/Net/Endpoint.h"
#include "SpehsEngine/Net/IOService.h"
#include "SpehsEngine/Net/PacketMessage.h"
#include "SpehsEngine/Net/EndpointUtilityFunctions.h"
#include <boost/asio/read.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <thread>
#include <mutex>

#define DEBUG_LOG(level, message) if (getDebugLogLevel() >= level) \
{ \
	se::log::info("SocketTCP(" + getLocalPort().toString() + "): " + message); \
}


namespace
{
#ifdef SE_FINAL_RELEASE
		static const se::time::Time handshakeReceiveTimeout = se::time::fromSeconds(5.0f);
		static const se::time::Time connectionTimeout = se::time::fromSeconds(5.0f);
#else
		static const se::time::Time handshakeReceiveTimeout = se::time::fromSeconds(10000.0f);
		static const se::time::Time connectionTimeout = se::time::fromSeconds(10000.0f);
#endif
}

namespace se
{
	struct Handshake
	{
		bool isValid() const
		{
			return handshakeVersion == currentVersion;
		}

		static const uint16_t currentVersion = 1;
		uint16_t handshakeVersion = currentVersion;
	};

	template<> template<typename S, typename T>
	static bool se::Serial<Handshake>::serial(S& _serial, T _value)
	{
		se_serial(_serial, _value.handshakeVersion, "handshakeVersion");
		return true;
	}

	extern std::string workingDirectory;
	namespace net
	{
		namespace
		{
			inline boost::asio::ip::tcp::endpoint toAsioEndpoint(const Endpoint& endpoint)
			{
				switch (endpoint.address.type)
				{
				case Address::Type::None: break;
				case Address::Type::Ipv4: return boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4(endpoint.address.data0), endpoint.port.value);
				case Address::Type::Ipv6:
				{
					boost::asio::ip::address_v6::bytes_type bytes;
					memcpy((void*)bytes[0], &endpoint.address.data0, 16);
					return boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v6(bytes), endpoint.port.value);
				}
				}
				return boost::asio::ip::tcp::endpoint();
			}

			inline Endpoint fromAsioEndpoint(const boost::asio::ip::tcp::endpoint& asioEndpoint)
			{
				Endpoint endpoint;
				if (asioEndpoint.address().is_v4())
				{
					endpoint.address = Address(asioEndpoint.address().to_v4().to_uint());
				}
				else if (asioEndpoint.address().is_v6())
				{
					const boost::asio::ip::address_v6::bytes_type bytes = asioEndpoint.address().to_v6().to_bytes();
					endpoint.address = Address(bytes[0]);
				}
				else
				{
					return Endpoint();
				}
				endpoint.port = Port(asioEndpoint.port());
				return endpoint;
			}
		}

		struct SocketTCP : public ISocketTCP
		{
			struct SharedImpl : public boost::enable_shared_from_this<SharedImpl>
			{
				SharedImpl(IOService& _ioService)
					: ioService(_ioService)
					, socket(getImplementationRef(_ioService))
					, receiveBuffer(512)
				{
				}

				~SharedImpl()
				{
					{
						std::lock_guard<std::recursive_mutex> lock(mutex);
						destructorCalled = true;
					}
					if (spehsAcceptThread)
					{
						spehsAcceptThread->join();
					}
				}

				bool open()
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					if (socket.is_open())
					{
						return true;
					}
					boost::system::error_code error;
					socket.open(boost::asio::ip::tcp::v4(), error);
					if (error)
					{
						if (debugLogLevel >= 1)
						{
							se::log::error("Failed to open SocketTCP. Boost asio error: " + error.message());
						}
						return false;
					}
					return true;
				}

				void close()
				{
					{
						std::lock_guard<std::recursive_mutex> lock(mutex);
						if (socket.is_open())
						{
							onReceiveCallback = std::function<void(BinaryReader&)>();
							boost::system::error_code error;
							socket.shutdown(boost::asio::socket_base::shutdown_both, error);
							if (error)
							{
								DEBUG_LOG(1, "failed to shutdown. Boost asio error on shutdown(): " + error.message());
							}
							socket.close(error);
							if (error)
							{
								DEBUG_LOG(1, "failed to close socket. Boost asio error on close(): " + error.message());
							}
							DEBUG_LOG(1, "closed.");
						}
						{
							std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
							if (receivedPackets.size() > 0)
							{
								se::log::warning("SocketTCP::close: " + std::to_string(receivedPackets.size()) + " unprocessed received packets will be cleared.");
							}
							clearReceivedPackets();
						}
					}
					while (isReceiving())
					{
						// Blocks
					}
				}

				bool bind(const Port& port)
				{
					if (!isOpen())
					{
						if (!open())
						{
							DEBUG_LOG(1, "failed to bind SocketTCP. Socket was not open and could not be opened.");
							return false;
						}
					}
					boost::system::error_code error;
					const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port.value);

					std::lock_guard<std::recursive_mutex> lock(mutex);
					socket.bind(endpoint, error);
					if (error)
					{
						DEBUG_LOG(1, "failed to bind SocketTCP. Boost asio error: " + error.message());
						return false;
					}
					DEBUG_LOG(1, "opened at port: " + port.toString());
					return true;
				}

				bool isOpen() const
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					return socket.is_open();
				}

				Port getLocalPort() const
				{
					return getLocalEndpoint().port;
				}

				Endpoint getLocalEndpoint() const
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					boost::system::error_code error;
					boost::asio::ip::tcp::endpoint localEndpoint = socket.local_endpoint(error);
					if (error)
					{
						if (debugLogLevel >= 1)
						{
							log::info("SocketTCP: failed to get local endpoint. Boost asio error: " + error.message());
						}
					}
					return fromAsioEndpoint(localEndpoint);
				}

				Address getRemoteAddress() const
				{
					return getRemoteEndpoint().address;
				}

				Port getRemotePort() const
				{
					return getRemoteEndpoint().port;
				}

				Endpoint getRemoteEndpoint() const
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					boost::system::error_code error;
					boost::asio::ip::tcp::endpoint remoteEndpoint = socket.remote_endpoint(error);
					if (error)
					{
						log::error("SocketTCP: failed to get remote endpoint. Boost asio error: " + error.message());
					}
					return fromAsioEndpoint(remoteEndpoint);
				}

				/* Boost initially passes received data to this receive handler. */
				void receiveHandler(const boost::system::error_code& error, std::size_t bytes)
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);

					if (!error)
					{
						DEBUG_LOG(2, "received " + std::to_string(bytes) + " bytes." + (error ? "Boost asio error: " + error.message() : ""));
						if (debugLogLevel >= 3)
						{
							for (size_t i = 0; i < bytes; i++)
							{
								log::info("    [" + std::to_string(i) + "] " + toHexString(receiveBuffer[i]));
							}
						}
					}

					receivedBytes += bytes;
					receiving = false;
					lastReceiveTime = time::now();


					if (error)
					{
						if (error == boost::asio::error::eof)
						{
							// Connection gracefully closed
							DEBUG_LOG(1, "disconnected. Remote socket closed connection.");
							if (socketTCP)
							{
								socketTCP->disconnect(DisconnectType::doNotSendDisconnectPacket);
							}
							return;
						}
						else if (error == boost::asio::error::connection_reset)
						{
							// Disconnect
							DEBUG_LOG(1, "SocketTCP disconnected. Remote socket closed connection.");
							if (socketTCP)
							{
								socketTCP->disconnect(DisconnectType::doNotSendDisconnectPacket);
							}
							return;
						}
						else if (error == boost::asio::error::connection_aborted ||
							error == boost::asio::error::connection_refused ||
							error == boost::asio::error::bad_descriptor ||
							error == boost::asio::error::operation_aborted)
						{
							DEBUG_LOG(1, "error while receiving. Boost asio error: (" + std::to_string(error.value()) + ") " + error.message() + ". Disconnecting.");

							if (socketTCP)
							{
								socketTCP->disconnect(DisconnectType::doNotSendDisconnectPacket);
							}

							return;
						}
						else
						{
							// Ignored error
							DEBUG_LOG(1, "error while receiving. Ignored boost asio error: " + std::to_string(error.value()) + ": " + error.message());
						}
					}

					if (bytes)
					{
						BinaryReader binaryReader(&receiveBuffer[0], bytes);
						if (expectedBytes == 0)
						{
							// Header received
							binaryReader.serial(expectedBytes);
							startReceiving();
						}
						else if (expectedBytes == bytes)
						{
							// Data received
							const bool keepReceiving = socketTCP ? socketTCP->spehsReceiveHandler(binaryReader) : false;
							expectedBytes = 0; // Begin to expect header next
							if (keepReceiving)
								startReceiving();
						}
						else
						{
							if (debugLogLevel >= 1)
							{
								log::error("SocketTCP received unexpected amount of bytes! Canceling receiving!");
							}
						}
					}
				}

				bool startAccepting(const Port& port, const std::function<void(ISocketTCP&)> callbackFunction)
				{
					if (isOpen())
					{
						DEBUG_LOG(1, "Failed to start accepting! Socket is already open.");
						return false;
					}
					std::lock_guard<std::recursive_mutex> lock(mutex);
					if (acceptingState != AcceptingState::idle)
					{
						DEBUG_LOG(1, "failed to start accepting! Socket is already accepting!");
						return false;
					}
					if (connected)
					{
						DEBUG_LOG(1, "failed start accepting! Socket is currently connected!");
						return false;
					}
					se_assert(!handshakeSent);
					se_assert(!handshakeReceived);
					onAcceptCallbackQueued = false;

					if (!acceptor)
						acceptor.reset(new boost::asio::ip::tcp::acceptor(getImplementationRef(ioService)));
					boost::system::error_code error;
					const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);

					// Open acceptor
					acceptor->open(endpoint.protocol(), error);
					if (error)
					{
						DEBUG_LOG(1, "failed to open acceptor! Boost asio error: " + error.message());
						return false;
					}

					// Bind acceptor
					acceptor->bind(endpoint, error);
					if (error)
					{
						DEBUG_LOG(1, "failed to bind acceptor! Boost asio error: " + error.message());
						return false;
					}

					// Make acceptor listen
					acceptor->listen(boost::asio::socket_base::max_connections, error);
					if (error)
					{
						DEBUG_LOG(1, "failed to make acceptor listen! Boost asio error: " + error.message());
						return false;
					}

					// Set the callback function
					onAcceptCallback = callbackFunction;

					// Start accepting asynchoronously
					acceptingState = AcceptingState::listeningForConnection;
					acceptor->async_accept(socket, boost::bind(&SharedImpl::onAccept, shared_from_this(), boost::asio::placeholders::error));

					return true;
				}

				/* Boost acceptor calls this method when an incoming connection is being accepted. If no error is detected, launches the synchronous spehsAccept method in a different thread (spehsAcceptThread). */
				void onAccept(const boost::system::error_code error)
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					if (socketTCP == nullptr)
						return;
					se_assert(!isConnected());
					se_assert(!isReceiving());
					se_assert(getAcceptingState() != AcceptingState::idle);
					se_assert(acceptor);
					acceptor->close();
					if (error)
					{
						DEBUG_LOG(1, "failed to accept an incoming connection! Boost asio error: " + error.message() + "Accepting has stopped.");
						socketTCP->disconnect(DisconnectType::doNotSendDisconnectPacket);
						acceptingState = AcceptingState::idle;
						return;
					}
					else
					{
						spehsAcceptThread.reset(new std::thread(&SharedImpl::spehsAccept, this));
					}
				}

				void spehsAccept()
				{
					// Start expecting an incoming handshake (connector sends first)
					{
						std::lock_guard<std::recursive_mutex> lock(mutex);
						if (!socketTCP)
						{
							return;
						}
						DEBUG_LOG(1, "accepting socket expecting a handshake...");
						acceptingState = AcceptingState::establishingConnection;
						startReceiving();
					}

					// NOTE: no mutex locking during the wait time
					waitForHandshake(handshakeReceiveTimeout);

					// Lock the mutex
					std::lock_guard<std::recursive_mutex> lock(mutex);
					if (!socketTCP)
					{
						return;
					}

					// Check if received the handshake in time
					if (!handshakeReceived)
					{
						DEBUG_LOG(1, "failed to accept an incoming connection! No response handshake received!");
						socketTCP->disconnect(DisconnectType::doNotSendDisconnectPacket);
						onAcceptCallbackQueued = true;
						return;
					}

					DEBUG_LOG(1, "accepting socket received a handshake.");

					// Send a response handshake
					BinaryWriter binaryWriter;
					const Handshake handshake;
					binaryWriter.serial(handshake);
					if (socketTCP->sendPacket(binaryWriter, PacketType::handshake))
					{
						DEBUG_LOG(1, "accepting socket sent a handshake.");
						handshakeSent = true;
					}
					else
					{
						DEBUG_LOG(1, "failed to accept an incoming connection! Could not send handshake!");
						socketTCP->disconnect(DisconnectType::doNotSendDisconnectPacket);
						onAcceptCallbackQueued = true;
						acceptingState = AcceptingState::idle;
						return;
					}

					// Socket is now in the connected state!
					acceptingState = AcceptingState::idle;
					connected = true;
					lastReceiveTime = time::now();
					onAcceptCallbackQueued = true;
				}

				bool waitForHandshake(const time::Time timeout)
				{
					const time::Time beginTime = time::now();
					while (true)
					{
						if (time::now() - beginTime > timeout)
							return false;
						std::lock_guard<std::recursive_mutex> lock(mutex);
						update();
						if (handshakeReceived)
							return true;
						if (destructorCalled)
							return false;
					}
				}

				void startReceiving()
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					if (receiving)
					{
						log::error("SocketTCP is already receiving.");
						return;
					}
					if (expectedBytes == 0)
					{
						// Receive header
						se_assert(receiveBuffer.size() >= sizeof(expectedBytes));
						boost::asio::async_read(socket, boost::asio::buffer(&receiveBuffer[0], sizeof(expectedBytes)),
							boost::bind(&SharedImpl::receiveHandler, shared_from_this(),
								boost::asio::placeholders::error,
								boost::asio::placeholders::bytes_transferred));
					}
					else
					{
						// Receive data
						if (receiveBuffer.size() < expectedBytes)
						{
							DEBUG_LOG(1, "the current receive buffer is insufficient! Expanding to " + std::to_string(expectedBytes));
							receiveBuffer.resize(expectedBytes);
						}
						boost::asio::async_read(socket, boost::asio::buffer(&receiveBuffer[0], expectedBytes),
							boost::bind(&SharedImpl::receiveHandler, shared_from_this(),
								boost::asio::placeholders::error,
								boost::asio::placeholders::bytes_transferred));
					}
					receiving = true;
				}

				void clearReceivedPackets()
				{
					std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
					receivedPackets.clear();
				}

				void stopAccepting()
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					if (acceptor)
					{
						boost::system::error_code error;
						acceptor->close(error);
						if (error)
						{
							DEBUG_LOG(1, "failed to stop accepting. Boost asio error: " + error.message());
						}
					}
				}

				bool connect(const Endpoint& endpoint)
				{
					if (isConnected())
					{
						DEBUG_LOG(1, "failed to connect. Socket is already connected.");
						return false;
					}

					{
						std::lock_guard<std::recursive_mutex> lock(mutex);

						// Start a new connection
						connecting = true;
						expectedBytes = 0;
						handshakeSent = false;
						handshakeReceived = false;

						// Resolve the remote endpoint
						boost::system::error_code error;
						boost::asio::ip::tcp::resolver resolverTCP(getImplementationRef(ioService));
						const boost::asio::ip::tcp::resolver::query query(endpoint.address.toString(), endpoint.port.toString());
						const boost::asio::ip::tcp::endpoint asioEndpoint = *resolverTCP.resolve(query, error);
						if (error)
						{
							DEBUG_LOG(1, "failed to resolve the endpoint. Boost asio error: " + error.message());
							connecting = false;
							return false;
						}
						else
						{
							DEBUG_LOG(1, "successfully resolved the remote endpoint.");
						}

						// Connect to the remote boost socket
						socket.connect(asioEndpoint, error);
						if (error)
						{
							DEBUG_LOG(1, "failed to connect(). Boost asio error: " + error.message());
							connecting = false;
							return false;
						}
						else
						{
							DEBUG_LOG(1, "successfully connected the remote boost socket.");
						}

						// Expect an incoming handshake after sending one
						startReceiving();

						// Send the spehs handshake
						BinaryWriter binaryWriter;
						const Handshake handshake;
						binaryWriter.serial(handshake);
						const time::Time handshakeSendBeginTime = time::now();
						while (time::now() - handshakeSendBeginTime < se::time::fromSeconds(5.0f))
						{
							if (sendPacket(binaryWriter, PacketType::handshake))
							{
								handshakeSent = true;
								break;
							}
						}
						if (handshakeSent)
						{
							DEBUG_LOG(1, "successfully sent handshake to the remote endpoint.");
						}
						else
						{
							DEBUG_LOG(1, "failed to connect. Failed to send handshake.");
							connecting = false;
							return false; // If sending the handshake fails, connection was not successful
						}
					}

					// Wait until received handshake
					waitForHandshake(handshakeReceiveTimeout);
					{
						std::lock_guard<std::recursive_mutex> lock(mutex);
						if (!handshakeReceived)
						{
							DEBUG_LOG(1, "failed to connect! No response handshake received!");
							connecting = false;
							return false;
						}
					}

					// All done, socket is now at connected state
					{
						DEBUG_LOG(1, "successfully received handshake from the remote endpoint. Socket is now in connected state.");
						std::lock_guard<std::recursive_mutex> lock(mutex);
						connecting = false;
						connected = true;
					}

					return true;
				}

				void disconnect()
				{
					disconnect(DisconnectType::unspecified);
				}

				void disconnect(const DisconnectType disconnectType)
				{
					if (!isConnected())
					{
						return;
					}

					if (disconnectType != DisconnectType::doNotSendDisconnectPacket)
					{
						// Try sending the disconnect packet before disconnecting
						BinaryWriter binaryWriter;
						binaryWriter.serial(disconnectType);
						sendPacket(binaryWriter, PacketType::disconnect);
					}

					// Reset the connection state
					std::lock_guard<std::recursive_mutex> lock(mutex);
					if (socket.is_open())
					{
						boost::system::error_code error;
						socket.shutdown(boost::asio::socket_base::shutdown_both, error);
						if (error)
						{
							DEBUG_LOG(1, "failed to shutdown. Boost asio error: " + error.message());
						}
						socket.close(error);
						if (error)
						{
							DEBUG_LOG(1, "failed to close. Boost asio error: " + error.message());
						}
					}
					connected = false;
					receiving = false;
					handshakeSent = false;
					handshakeReceived = false;
				}

				bool sendPacket(const BinaryWriter& binaryWriter, const PacketType packetType)
				{
					if (binaryWriter.getSize() == 0)
					{
						se_assert(false && "Trying to send an empty packet.");
						return true;
					}

					std::lock_guard<std::recursive_mutex> lock(mutex);
					boost::system::error_code error;

					if (!connected && !connecting && acceptingState == ISocketTCP::AcceptingState::idle)
					{
						// Can only send user defined packets in the connected state
						DEBUG_LOG(1, "cannot send a packet. Socket is neither connected, connecting nor accepting.");
						return false;
					}

					// Spehs header
					BinaryWriter headerBinaryWriter;
					const ISocketTCP::ExpectedBytesType dataBufferSize = ISocketTCP::ExpectedBytesType(binaryWriter.getSize());
					const ISocketTCP::ExpectedBytesType headerBytesValue = ISocketTCP::ExpectedBytesType(binaryWriter.getSize() + sizeof(packetType));

					headerBinaryWriter.serial(headerBytesValue);
					headerBinaryWriter.serial(packetType);

					const size_t headerSize = headerBinaryWriter.getSize();
					size_t offset = 0;
					while (offset < headerSize)
					{
						// Keep sending data until the whole header has been sent
						const size_t writtenBytes = socket.write_some(boost::asio::buffer(headerBinaryWriter.getData() - offset, headerSize - offset), error);
						offset += writtenBytes;
						sentBytes += writtenBytes;
						if (error)
						{
							// Error occured while sending data...
							DEBUG_LOG(0, "failed to send packet's spehs header! Boost asio error: " + error.message());
							return false;
						}
					}

					// Data
					offset = 0;
					while (offset < dataBufferSize)
					{
						// Keep sending data until all data has been sent
						const size_t writtenBytes = socket.write_some(boost::asio::buffer(binaryWriter.getData() + offset, dataBufferSize - offset), error);
						offset += writtenBytes;
						sentBytes += writtenBytes;
						if (error)
						{
							//Error occured while sending data...
							DEBUG_LOG(0, "SocketTCP: failed to send packet! Boost asio error: " + error.message());
							return false;
						}
					}

					DEBUG_LOG(2, "sent 4(packet byte size) + 1(packet type) + " + std::to_string(binaryWriter.getSize()) + " bytes to: " + getLocalEndpoint().toString());

					return true;
				}

				bool placeDataOnReceiveQueue(BinaryWriter& binaryWriter)
				{
					if (isConnected())
					{
						std::lock_guard<std::recursive_mutex> lock(mutex);
						std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
						receivedPackets.push_back(std::make_unique<ReceivedPacket>());
						binaryWriter.swap(receivedPackets.back()->data);
						return true;
					}
					else
					{
						return false;
					}
				}

				bool resizeReceiveBuffer(const size_t newSize)
				{
					if (newSize < sizeof(ISocketTCP::ExpectedBytesType))
						return false;
					std::lock_guard<std::recursive_mutex> lock(mutex);
					if (receiving)
						return false;
					receiveBuffer.resize(newSize);
					return true;
				}

				void setOnReceiveCallback(const std::function<void(BinaryReader&)> callbackFunction)
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					onReceiveCallback = callbackFunction;
				}

				bool hasOnReceiveCallback() const
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					return bool(onReceiveCallback);
				}

				void update()
				{
					{
						// onAccept callback
						std::lock_guard<std::recursive_mutex> lock(mutex);
						if (onAcceptCallbackQueued)
						{
							onAcceptCallbackQueued = false;
							if (socketTCP)
								onAcceptCallback(*socketTCP);
						}
					}

					/*
						Processing received packets
							-One at the time
							-Do not leave mutex locked for the duration of receive handler
							-Assume that receive handler might change/reset during receive handling
					*/
					while (true)
					{
						std::vector<uint8_t> data;
						std::function<void(BinaryReader&)> handler;
						{
							std::lock_guard<std::recursive_mutex> lock(mutex);
							std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
							if (receivedPackets.empty() || !onReceiveCallback)
							{
								break;
							}
							else
							{
								// Copy contents to non-mutex protected memory
								data.swap(receivedPackets.front().get()->data);
								handler = onReceiveCallback;
								receivedPackets.erase(receivedPackets.begin());
							}
						}
						BinaryReader binaryReader(data.data(), data.size());
						handler(binaryReader);
					}
				}

				bool spehsReceiveHandler(BinaryReader& binaryReader)
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);

					// Read packet type
					PacketType packetType;
					binaryReader.serial(packetType);
					DEBUG_LOG(2, "received packet of type: " + std::to_string((int)packetType) + ", " + std::to_string(binaryReader.getBytesRemaining()) + " bytes.");

					// Process packet
					switch (packetType)
					{
					default:
						break;
					case PacketType::undefined:
					{
						const size_t userBytes = binaryReader.getBytesRemaining();
						DEBUG_LOG(2, "received user defined packet. Bytes: " + std::to_string(userBytes));
						se_assert(userBytes > 0);

						// Push to received packets queue
						std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
						receivedPackets.push_back(std::unique_ptr<ReceivedPacket>(new ReceivedPacket(userBytes)));
						memcpy(receivedPackets.back()->data.data(), binaryReader.getData() + binaryReader.getOffset(), userBytes);
						return true;
					}
					case PacketType::disconnect:
					{
						DisconnectType disconnectType;
						binaryReader.serial(disconnectType);
						disconnect(DisconnectType::doNotSendDisconnectPacket);
						DEBUG_LOG(1, "remote socket gracefully disconnected. Disconnect type was: " + std::to_string((uint8_t)disconnectType));
						return false;
					}
					case PacketType::handshake:
					{
						if (handshakeReceived)
						{
							log::warning("A handshake for the current connection has already been received!");
							return false;
						}
						Handshake handshake;
						if (binaryReader.serial(handshake) && handshake.isValid())
						{
							DEBUG_LOG(1, "valid spehs handshake received.");
							handshakeReceived = true;
						}
						else
						{
							DEBUG_LOG(1, "received an invalid spehs handshake!");
						}
						return true;
					}
					}
					return false;
				}

				void setNoDelay(const bool enabled)
				{
					se_assert(isOpen());
					const boost::asio::ip::tcp::no_delay option(true);
					boost::system::error_code error;
					std::lock_guard<std::recursive_mutex> lock1(mutex);
					socket.set_option(option, error);
					if (error)
					{
						DEBUG_LOG(1, "failed to set no_delay option. Boost asio error: " + error.message());
					}
				}

				bool getNoDelay() const
				{
					boost::asio::ip::tcp::no_delay option;
					boost::system::error_code error;
					std::lock_guard<std::recursive_mutex> lock1(mutex);
					socket.get_option(option, error);
					if (error)
					{
						DEBUG_LOG(1, "failed to get no_delay option. Boost asio error: " + error.message());
					}
					return option;
				}

				void setReuseAddress(const bool enabled)
				{
					se_assert(isOpen());
					boost::system::error_code error;
					std::lock_guard<std::recursive_mutex> lock1(mutex);
					socket.set_option(boost::asio::socket_base::reuse_address(enabled), error);
					if (error)
					{
						DEBUG_LOG(1, "failed to set reuse_address option. Boost asio error: " + error.message());
					}
				}

				bool getReuseAddress() const
				{
					boost::asio::socket_base::reuse_address option;
					boost::system::error_code error;
					std::lock_guard<std::recursive_mutex> lock1(mutex);
					socket.get_option(option, error);
					if (error)
					{
						DEBUG_LOG(1, "failed to get reuse_address option. Boost asio error: " + error.message());
					}
					return option;
				}

				void setKeepAlive(const bool enabled)
				{
					const boost::asio::socket_base::keep_alive option(true);
					boost::system::error_code error;
					std::lock_guard<std::recursive_mutex> lock1(mutex);
					socket.set_option(option, error);
					if (error)
					{
						DEBUG_LOG(1, "failed to set keep_alive option. Boost asio error: " + error.message());
					}
				}

				bool getKeepAlive() const
				{
					boost::asio::socket_base::keep_alive option;
					boost::system::error_code error;
					std::lock_guard<std::recursive_mutex> lock1(mutex);
					socket.get_option(option, error);
					if (error)
					{
						DEBUG_LOG(1, "failed to get keep_alive option. Boost asio error: " + error.message());
					}
					return option;
				}

				SocketTCP::AcceptingState getAcceptingState() const
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					return acceptingState;
				}

				bool isAccepting() const
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					return acceptingState != SocketTCP::AcceptingState::idle;
				}

				bool isReceiving() const
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					return receiving;
				}

				bool isConnected() const
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					if (time::now() - lastReceiveTime >= connectionTimeout)
						return false;
					return connected;
				}

				size_t getSentBytes() const
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					return sentBytes;
				}

				size_t getReceivedBytes() const
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					return receivedBytes;
				}

				void setDebugLogLevel(const int level)
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					debugLogLevel = level;
				}

				int getDebugLogLevel() const
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					return debugLogLevel;
				}

				mutable std::recursive_mutex mutex;
				IOService& ioService;
				boost::asio::ip::tcp::socket socket;
				std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;
				std::unique_ptr<std::thread> spehsAcceptThread;
				std::function<void(BinaryReader&)> onReceiveCallback; // User defined receive handler
				std::function<void(ISocketTCP&)> onAcceptCallback;
				ISocketTCP::ExpectedBytesType expectedBytes = 0;
				std::vector<unsigned char> receiveBuffer;
				time::Time lastReceiveTime;
				bool receiving = false;
				AcceptingState acceptingState = AcceptingState::idle;
				bool connected = false;
				bool connecting = false;//Set to true for the duration of connect attempt
				bool handshakeSent = false;//Refers to the current connection
				bool handshakeReceived = false;//Refers to the current connection
				bool onAcceptCallbackQueued = false;//If enabled, update will invoke the callback.
				bool destructorCalled = false;
				SocketTCP* socketTCP = nullptr;
				size_t sentBytes = 0;
				size_t receivedBytes = 0;

				//Received packets
				std::recursive_mutex receivedPacketsMutex;
				struct ReceivedPacket
				{
					ReceivedPacket() = default;
					ReceivedPacket(const size_t size) : data(size) {}
					std::vector<uint8_t> data;
				};
				std::vector<std::unique_ptr<ReceivedPacket>> receivedPackets;

				/*
					level 1: prints most essential state changes.
					level 2: prints some network traffic numbers.
					level 3: prints receive buffer in hex string.
				*/
				int debugLogLevel = 0;
			};

			SocketTCP(IOService& _ioService)
				: sharedImpl(new SharedImpl(_ioService))
			{
				sharedImpl->socketTCP = this;
			}

			~SocketTCP()
			{
				if (isConnected())
					disconnect(DisconnectType::socketDestructor);
				stopAccepting();
				close();
				std::lock_guard<std::recursive_mutex> lock1(sharedImpl->mutex);
				se_assert(sharedImpl->socketTCP);
				sharedImpl->socketTCP = nullptr;
			}

			bool open()
			{
				return sharedImpl->open();
			}

			void close()
			{
				sharedImpl->disconnect();
				sharedImpl->close();
			}

			bool bind(const Port& port)
			{
				return sharedImpl->bind(port);
			}

			bool isOpen() const
			{
				return sharedImpl->isOpen();
			}

			void waitUntilFinishedReceiving()
			{
				while (isReceiving()) { }
			}

			void waitUntilFinishedAccepting()
			{
				while (getAcceptingState() != AcceptingState::idle) { }
			}

			bool waitForHandshake(const time::Time timeout)
			{
				return sharedImpl->waitForHandshake(timeout);
			}

			void clearReceivedPackets()
			{
				sharedImpl->clearReceivedPackets();
			}

			bool connect(const Endpoint& endpoint)
			{
				return sharedImpl->connect(endpoint);
			}

			void disconnect()
			{
				sharedImpl->disconnect(DisconnectType::unspecified);
			}

			void disconnect(const DisconnectType disconnectType)
			{
				sharedImpl->disconnect(disconnectType);
			}

			void stopAccepting()
			{
				sharedImpl->stopAccepting();
			}

			size_t getSentBytes() const
			{
				return sharedImpl->getSentBytes();
			}

			size_t getReceivedBytes() const
			{
				return sharedImpl->getReceivedBytes();
			}

			bool sendPacket(const BinaryWriter& binaryWriter, const PacketType packetType)
			{
				return sharedImpl->sendPacket(binaryWriter, packetType);
			}

			bool placeDataOnReceiveQueue(BinaryWriter& binaryWriter)
			{
				return sharedImpl->placeDataOnReceiveQueue(binaryWriter);
			}

			bool resizeReceiveBuffer(const size_t newSize)
			{
				return sharedImpl->resizeReceiveBuffer(newSize);
			}

			void setOnReceiveCallback(const std::function<void(BinaryReader&)> callbackFunction)
			{
				sharedImpl->setOnReceiveCallback(callbackFunction);
			}

			bool hasOnReceiveCallback() const
			{
				return sharedImpl->hasOnReceiveCallback();
			}

			void startReceiving()
			{
				sharedImpl->startReceiving();
			}

			void update()
			{
				return sharedImpl->update();
			}

			bool spehsReceiveHandler(BinaryReader& binaryReader)
			{
				return sharedImpl->spehsReceiveHandler(binaryReader);
			}

			bool startAccepting(const Port& port, const std::function<void(ISocketTCP&)> callback) final
			{
				return sharedImpl->startAccepting(port, callback);
			}

			void setNoDelay(const bool enabled)
			{
				sharedImpl->setNoDelay(enabled);
			}

			bool getNoDelay() const
			{
				return sharedImpl->getNoDelay();
			}

			void setReuseAddress(const bool enabled)
			{
				sharedImpl->setReuseAddress(enabled);
			}

			bool getReuseAddress() const
			{
				return sharedImpl->getReuseAddress();
			}

			void setKeepAlive(const bool enabled)
			{
				sharedImpl->setKeepAlive(enabled);
			}

			bool getKeepAlive() const
			{
				return sharedImpl->getKeepAlive();
			}

			Address getRemoteAddress() const
			{
				return sharedImpl->getRemoteAddress();
			}

			Port getRemotePort() const
			{
				return sharedImpl->getRemotePort();
			}

			Endpoint getRemoteEndpoint() const
			{
				return sharedImpl->getRemoteEndpoint();
			}

			Port getLocalPort() const
			{
				return sharedImpl->getLocalPort();
			}

			Endpoint getLocalEndpoint() const
			{
				return sharedImpl->getLocalEndpoint();
			}

			AcceptingState getAcceptingState() const
			{
				return sharedImpl->getAcceptingState();
			}

			bool isAccepting() const
			{
				return sharedImpl->isAccepting();
			}

			bool isReceiving() const
			{
				return sharedImpl->isReceiving();
			}

			bool isConnected() const
			{
				return sharedImpl->isConnected();
			}

			void setDebugLogLevel(const int level)
			{
				sharedImpl->setDebugLogLevel(level);
			}

			int getDebugLogLevel() const
			{
				return sharedImpl->getDebugLogLevel();
			}

			boost::shared_ptr<SharedImpl> sharedImpl;
		};

		std::unique_ptr<ISocketTCP> ISocketTCP::create(IOService& ioService)
		{
			return std::unique_ptr<ISocketTCP>(new SocketTCP(ioService));
		}
	}
}
