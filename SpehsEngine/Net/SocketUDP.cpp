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

		SocketUDP::SocketUDP(IOService& _ioService)
			: sharedImpl(new SharedImpl(_ioService))
		{
			sharedImpl->socketUDP = this;
		}

		SocketUDP::~SocketUDP()
		{
			close();
			std::lock_guard<std::recursive_mutex> lock1(sharedImpl->mutex);
			se_assert(sharedImpl->socketUDP);
			sharedImpl->socketUDP = nullptr;
		}

		bool SocketUDP::open()
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			boost::system::error_code error;
			sharedImpl->socket.open(boost::asio::ip::udp::v4(), error);
			if (error)
			{
				se::log::error("Failed to open SocketUDP. Boost asio error: " + error.message());
				return false;
			}
			return true;
		}

		void SocketUDP::close()
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			if (sharedImpl->socket.is_open())
			{
				sharedImpl->socket.shutdown(boost::asio::socket_base::shutdown_both);
				sharedImpl->socket.close();
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

			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			sharedImpl->socket.bind(endpoint, error);
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
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			boost::asio::ip::udp::resolver resolverUDP(sharedImpl->ioService.getImplementationRef());
			const boost::asio::ip::udp::resolver::query queryUDP(boost::asio::ip::udp::v4(), remoteEndpoint.address.toString(), remoteEndpoint.port.toString());
			const boost::asio::ip::udp::endpoint remoteAsioEndpoint = *resolverUDP.resolve(queryUDP);
			boost::system::error_code error;
			sharedImpl->socket.connect(remoteAsioEndpoint, error);
			if (error)
			{
				log::info("SocketUDP connect() failed(). Boost asio error: " + std::to_string(error.value()) + ": " + error.message());
				return false;
			}
			else
			{
				sharedImpl->connectedEndpoint = remoteAsioEndpoint;
				log::info("SocketUDP successfully connected to the remote endpoint at: " + remoteEndpoint.toString() + " at local port: " + std::to_string(sharedImpl->socket.local_endpoint().port()));
				return true;
			}
		}

		void SocketUDP::disconnect()
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			se_assert(getConnectedEndpoint());
			sharedImpl->connectedEndpoint = boost::asio::ip::udp::endpoint();
		}
		
		void SocketUDP::clearReceivedPackets()
		{
			std::lock_guard<std::recursive_mutex> lock1(sharedImpl->mutex);
			std::lock_guard<std::recursive_mutex> lock2(sharedImpl->receivedPacketsMutex);
			sharedImpl->receivedPackets.clear();
		}

		void SocketUDP::stopReceiving()
		{
			{
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				if (sharedImpl->socket.is_open())
				{
					boost::system::error_code error;
					sharedImpl->socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
					if (error)
					{
						log::info("SocketUDP: boost asio error on shutdown(): " + error.message());
					}
					sharedImpl->socket.close(error);//TODO: this actually cancels all asynchronous operations, not just receiving...
					if (error)
					{
						log::info("SocketUDP: boost asio error on close(): " + error.message());
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
			if (isConnected())
			{
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				return sendPacketInternal(buffer, sharedImpl->connectedEndpoint);
			}
			else
			{
				se_assert(false && "connect() hasn't been called.");
				return false;
			}
		}

		bool SocketUDP::sendPacket(const WriteBuffer& buffer, const Endpoint& endpoint)
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			boost::system::error_code error;

			boost::asio::ip::udp::resolver resolver(sharedImpl->ioService.getImplementationRef());
			const boost::asio::ip::udp::resolver::query query(endpoint.address.toString(), endpoint.port.toString());
			const boost::asio::ip::udp::endpoint asioEndpoint = *resolver.resolve(query, error);
			if (error)
			{
				se::log::error("SocketUDP send failed.");
				return false;
			}

			return sendPacketInternal(buffer, asioEndpoint);
		}

		bool SocketUDP::sendPacketInternal(const WriteBuffer& buffer, const boost::asio::ip::udp::endpoint& endpoint)
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			boost::system::error_code error;
			const ExpectedBytesType bytesSent = sharedImpl->socket.send_to(boost::asio::buffer(buffer[0], buffer.getOffset()), endpoint, 0, error);
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
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			if (sharedImpl->receiving)
				return false;
			sharedImpl->receiveBuffer.resize(newSize);
			return true;
		}

		bool SocketUDP::startReceiving(const std::function<void(ReadBuffer&, const Endpoint& endpoint)> callbackFunction)
		{
			if (!callbackFunction)
			{
				log::info("SocketUDP failed to start receiving. No callback function specified.");
				return false;
			}

			if (isReceiving())
			{
				log::info("SocketUDP failed to start receiving. Socket is already receiving.");
				return false;
			}

			if (!isOpen())
			{
				log::info("SocketUDP failed to start receiving. Socket has not been opened.");
				return false;
			}

			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			sharedImpl->receiving = true;
			sharedImpl->lastReceiveTime = time::now();
			sharedImpl->onReceiveCallback = callbackFunction;
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
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			if (isConnected())
			{
				try
				{
					sharedImpl->socket.async_receive(boost::asio::buffer(sharedImpl->receiveBuffer),
						boost::bind(&SocketUDP::SharedImpl::receiveHandler, sharedImpl->shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
				}
				catch (const std::exception& e)
				{
					log::error(std::string("Exception thrown: ") + e.what());
				}
			}
			else
			{
				try
				{
					sharedImpl->socket.async_receive_from(boost::asio::buffer(sharedImpl->receiveBuffer), sharedImpl->senderEndpoint,
						boost::bind(&SocketUDP::SharedImpl::receiveHandler, sharedImpl->shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
				}
				catch (const std::exception& e)
				{
					log::error(std::string("Exception thrown: ") + e.what());
				}
			}
		}

		void SocketUDP::update()
		{
			//Received packets
			std::lock_guard<std::recursive_mutex> lock1(sharedImpl->mutex);
			std::lock_guard<std::recursive_mutex> lock2(sharedImpl->receivedPacketsMutex);
			if (sharedImpl->onReceiveCallback)
			{
				for (size_t i = 0; i < sharedImpl->receivedPackets.size(); i++)
				{
					ReadBuffer readBuffer(sharedImpl->receivedPackets[i]->buffer.data(), sharedImpl->receivedPackets[i]->buffer.size());
					const Endpoint endpoint(
						Address(Address::ValueType(sharedImpl->receivedPackets[i]->senderEndpoint.address().to_string())),
						Port(Port::ValueType(sharedImpl->receivedPackets[i]->senderEndpoint.port())));
					sharedImpl->onReceiveCallback(readBuffer, endpoint);
				}
			}
			clearReceivedPackets();
		}

		bool SocketUDP::isReceiving() const
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			return sharedImpl->receiving;
		}

		bool SocketUDP::isOpen() const
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			return sharedImpl->socket.is_open();
		}

		Port SocketUDP::getLocalPort() const
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			if (sharedImpl->socket.is_open())
				return Port(sharedImpl->socket.local_endpoint().port());
			else
				return Port::invalid;
		}

		bool SocketUDP::isConnected() const
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			return sharedImpl->connectedEndpoint != boost::asio::ip::udp::endpoint();
		}

		Endpoint SocketUDP::getConnectedEndpoint() const
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			if (sharedImpl->connectedEndpoint == boost::asio::ip::udp::endpoint())
				return Endpoint::invalid;
			else
				return Endpoint(Address(Address::ValueType(sharedImpl->connectedEndpoint.address().to_string())), Port(Port::ValueType(sharedImpl->connectedEndpoint.port())));
		}
		
		SocketUDP::SharedImpl::SharedImpl(IOService& _ioService)
			: ioService(_ioService)
			, socket(_ioService.getImplementationRef())
			, receiveBuffer(65536)
		{

		}

		void SocketUDP::SharedImpl::receiveHandler(const boost::system::error_code& error, std::size_t bytes)
		{
			if (debugLogLevel >= 2)
				log::info("SocketUDP receive handler received " + std::to_string(bytes) + " bytes.");

			std::lock_guard<std::recursive_mutex> lock1(mutex);
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
				if (error == boost::asio::error::operation_aborted)
				{
					log::info("SocketUDP: boost asio error: operation_aborted");
					return;
				}
				else if (error == boost::asio::error::eof)
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
					error == boost::asio::error::bad_descriptor)
				{
					log::info("Boost asio error: " + std::to_string(error.value()));
					if (error == boost::asio::error::connection_aborted)
						log::info("SocketUDP: boost asio error: connection_aborted");
					if (error == boost::asio::error::connection_refused)
						log::info("SocketUDP: boost asio error: connection_refused");
					if (error == boost::asio::error::bad_descriptor)
						log::info("SocketUDP: boost asio error: bad_descriptor");
					se_assert(false && "Should this ever happen with the UDP socket?");
					return;
				}
				else
				{//Ignored error
					log::info("Ignored ASIO error: " + std::to_string(error.value()) + ": " + error.message());
				}
			}

			se_assert(socketUDP);
			if (bytes > 0)
			{
				ReadBuffer readBuffer(&receiveBuffer[0], bytes);
				std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
				receivedPackets.push_back(std::unique_ptr<ReceivedPacket>());
				receivedPackets.back().reset(new ReceivedPacket());
				receivedPackets.back()->buffer.resize(bytes);
				memcpy(receivedPackets.back()->buffer.data(), receiveBuffer.data(), bytes);
				if (socketUDP->isConnected())
				{
					receivedPackets.back()->senderEndpoint = connectedEndpoint;
				}
				else
				{
					receivedPackets.back()->senderEndpoint = senderEndpoint;
				}
			}

			socketUDP->resumeReceiving();
		}
	}
}