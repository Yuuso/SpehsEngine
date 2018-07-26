#include "stdafx.h"
#include "SpehsEngine/Net/SocketUDP.h"

#include <assert.h>
#include <iostream>
#include <string>
#include "SpehsEngine/Net/Acceptor.h"
#include "SpehsEngine/Net/IOService.h"
#include "SpehsEngine/Net/Handshake.h"
#include <SpehsEngine/Core/WriteBuffer.h>
#include <SpehsEngine/Core/ReadBuffer.h>
#include "SpehsEngine/Core/RAIIVariableSetter.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Core/StringOperations.h"

namespace
{
	/*
	level 1: prints most essential state changes.
	level 2: prints some network traffic numbers.
	level 3: prints receive buffer in hex string.
	*/
	int debugLogLevel = 0;
}

namespace se
{
	extern std::string workingDirectory;
	namespace net
	{
		static const time::Time handshakeReceiveTimeout = time::fromSeconds(10000);
		static const time::Time connectionTimeout = time::fromSeconds(10000);

		SocketUDP::Id SocketUDP::nextId = 0;
		SocketUDP::SocketUDP(IOService& _ioService)
			: id(nextId++)
			, ioService(_ioService)
			, socket(_ioService.getImplementationRef())
			, receiveBuffer(65536)
		{

		}

		SocketUDP::~SocketUDP()
		{
			waitUntilFinishedReceiving();
			std::lock_guard<std::recursive_mutex> lock(mutex);

			clearReceivedPackets();
			close();
		}

		bool SocketUDP::open()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			boost::system::error_code error;
			socket.open(boost::asio::ip::udp::v4(), error);
			if (error)
			{
				se::log::error("Failed to open SocketUDP. Boost asio error: " + error.message());
				return false;
			}
			return true;
		}

		void SocketUDP::close()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (socket.is_open())
			{
				socket.shutdown(boost::asio::socket_base::shutdown_both);
				socket.close();
				se::log::info("SocketUDP closed.");
			}
		}

		bool SocketUDP::bind(const Port& port)
		{
			if (!isOpen())
			{
				se_assert(false && "Socket must be opened first.");
				return false;
			}
			boost::system::error_code error;
			const boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::udp::v4(), port.value);
			socket.bind(endpoint, error);
			if (error)
			{
				se::log::error("Failed to bind SocketUDP. Boost asio error: " + error.message());
				return false;
			}
			se::log::info("SocketUDP opened at port: " + port.toString());
			return true;
		}

		bool SocketUDP::connect(const Endpoint& remoteEndpoint)
		{
			if (!isOpen())
			{
				log::error("Cannot connect UDP socket. Socket has not been opened.");
				return false;
			}
			boost::asio::ip::udp::resolver resolverUDP(ioService.getImplementationRef());
			const boost::asio::ip::udp::resolver::query queryUDP(boost::asio::ip::udp::v4(), remoteEndpoint.address.toString(), remoteEndpoint.port.toString());
			const boost::asio::ip::udp::endpoint serverEndpointUDP = *resolverUDP.resolve(queryUDP);
			boost::system::error_code error;
			socket.connect(serverEndpointUDP, error);
			if (error)
			{
				log::info("SocketUDP connect() failed(). Boost asio error: " + std::to_string(error.value()) + ": " + error.message());
				return false;
			}
			else
			{
				connectedEndpoint = remoteEndpoint;
				log::info("SocketUDP successfully connected to the remote endpoint at: " + remoteEndpoint.toString());
				return true;
			}
		}

		void SocketUDP::disconnect()
		{
			se_assert(getConnectedEndpoint());
			connectedEndpoint = Endpoint();
		}

		void SocketUDP::waitUntilFinishedReceiving()
		{
			bool wait = true;
			while (wait)
			{
				wait = isReceiving();
			}
		}
		
		void SocketUDP::clearReceivedPackets()
		{
			std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
			for (size_t i = 0; i < receivedPackets.size(); i++)
				delete receivedPackets[i];
			receivedPackets.clear();
		}

		void SocketUDP::stopReceiving()
		{
			{
				std::lock_guard<std::recursive_mutex> lock(mutex);
				if (socket.is_open())
				{
					try
					{
						socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
						socket.close();//TODO: this actually cancels all asynchronous operations, not just receiving...
					}
					catch (std::exception& e)
					{
						log::info(e.what());
					}
				}
			}
			while (isReceiving())
			{
				//Blocks
			}
		}

		bool SocketUDP::sendPacket(const WriteBuffer& buffer)
		{
			if (connectedEndpoint)
			{
				return sendPacket(buffer, connectedEndpoint);
			}
			else
			{
				se_assert(false && "connect() hasn't been called.");
				return false;
			}
		}

		bool SocketUDP::sendPacket(const WriteBuffer& buffer, const Endpoint& endpoint)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			boost::system::error_code error;			
			
			boost::asio::ip::udp::resolver resolver(ioService.getImplementationRef());
			const boost::asio::ip::udp::resolver::query query(endpoint.address.toString(), endpoint.port.toString());
			const boost::asio::ip::udp::endpoint asioEndpoint = *resolver.resolve(query, error);

			const ExpectedBytesType bytesSent = socket.send_to(boost::asio::buffer(buffer[0], buffer.getOffset()), asioEndpoint, 0, error);
			if (error)
				se::log::error("SocketUDP send failed.");
			if (bytesSent != buffer.getOffset())
				se::log::error("SocketUDP send failed.");
			
			if (debugLogLevel >= 2)
				log::info("SocketUDP: packet sent. Contents: 4(packet byte size) + 1(packet type) + " + std::to_string(buffer.getOffset()) + "(data)");
			return true;
		}

		bool SocketUDP::resizeReceiveBuffer(const size_t newSize)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (receiving)
				return false;
			receiveBuffer.resize(newSize);
			return true;
		}

		bool SocketUDP::startReceiving(const std::function<void(ReadBuffer&, const Endpoint& endpoint)> callbackFunction)
		{
			if (isReceiving())
			{
				log::info("SocketUDP failed to start receiving. Socket is already receiving.");
				return false;
			}

			if (!callbackFunction)
			{
				log::info("SocketUDP failed to start receiving. No callback function specified.");
				return false;
			}

			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (!socket.is_open())
			{//This should never happen as the connected status should guarantee socket being open...
				log::info("SocketUDP failed to start receiving. Socket has not been opened.");
				return false;
			}

			receiving = true;
			lastReceiveTime = time::now();
			onReceiveCallback = callbackFunction;
			clearReceivedPackets();
			resumeReceiving();

			if (debugLogLevel >= 2)
			{
				log::info("SocketUDP successfully started receiving.");
			}

			return true;
		}

		void SocketUDP::resumeReceiving()
		{
			socket.async_receive_from(boost::asio::buffer(receiveBuffer), senderEndpoint,
				boost::bind(&SocketUDP::receiveHandler,
					this, boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}

		void SocketUDP::update()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			
			//Received packets
			std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
			if (onReceiveCallback)
			{
				for (size_t i = 0; i < receivedPackets.size(); i++)
				{
					ReadBuffer readBuffer(receivedPackets[i]->buffer.data(), receivedPackets[i]->buffer.size());
					onReceiveCallback(readBuffer, receivedPackets[i]->senderEndpoint);
				}
			}
			clearReceivedPackets();
		}

		void SocketUDP::receiveHandler(const boost::system::error_code& error, std::size_t bytes)
		{
			if (debugLogLevel >= 2)
				log::info("SocketUDP receive handler received " + std::to_string(bytes) + " bytes.");

			std::lock_guard<std::recursive_mutex> lock(mutex);
			receiving = false;
			lastReceiveTime = time::now();

			if (debugLogLevel >= 3)
			{
				log::info("Received bytes: ");
				for (size_t i = 0; i < bytes; i++)
					log::info("    [" + std::to_string(i) + "] " + toHexString(receiveBuffer[i]));
			}

			if (error)
			{
				if (error == boost::asio::error::eof)
				{//Connection gracefully closed
					log::info("SocketUDP disconnected. Remote socket closed connection.");
					se_assert(false && "Should this ever happen with the UDP socket?");
					return;
				}
				else if (error == boost::asio::error::connection_reset)
				{//Disconnect
					log::info("SocketUDP disconnected. Remote socket closed connection.");
					se_assert(false && "Should this ever happen with the UDP socket?");
					return;
				}
				else if (error == boost::asio::error::connection_aborted ||
					error == boost::asio::error::connection_refused ||
					error == boost::asio::error::bad_descriptor ||
					error == boost::asio::error::operation_aborted)
				{
					log::info("Boost asio error: " + std::to_string(error.value()));
					if (error == boost::asio::error::connection_aborted)
						log::info("Closing client: boost asio error: connection_aborted");
					if (error == boost::asio::error::connection_refused)
						log::info("Closing client: boost asio error: connection_refused");
					if (error == boost::asio::error::bad_descriptor)
						log::info("Closing client: boost asio error: bad_descriptor");
					if (error == boost::asio::error::operation_aborted)
						log::info("Closing client: boost asio error: operation_aborted");
					se_assert(false && "Should this ever happen with the UDP socket?");
					return;
				}
				else
				{//Ignored error
					log::info("Ignored ASIO error: " + std::to_string(error.value()) + ": " + error.message());
				}
			}

			if (bytes > 0)
			{
				ReadBuffer readBuffer(&receiveBuffer[0], bytes);
				std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
				receivedPackets.push_back(new ReceivedPacket());
				receivedPackets.back()->buffer.resize(bytes);
				memcpy(receivedPackets.back()->buffer.data(), receiveBuffer.data(), bytes);
				receivedPackets.back()->senderEndpoint.address.value = senderEndpoint.address().to_string();
				receivedPackets.back()->senderEndpoint.port.value = senderEndpoint.port();
			}

			resumeReceiving();
		}

		bool SocketUDP::isReceiving() const
		{
			std::lock_guard<std::recursive_mutex> locks(mutex);
			return receiving;
		}

		bool SocketUDP::isOpen() const
		{
			return socket.is_open();
		}

		Port SocketUDP::getLocalPort() const
		{
			if (socket.is_open())
				return Port(socket.local_endpoint().port());
			else
				return Port::invalid;
		}

		Endpoint SocketUDP::getConnectedEndpoint() const
		{
			return connectedEndpoint;
		}
	}
}