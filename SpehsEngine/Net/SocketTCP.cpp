#include "stdafx.h"
#include "SpehsEngine/Net/SocketTCP.h"

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

		SocketTCP::SocketTCP(IOService& _ioService)
			: sharedImpl(new SharedImpl(_ioService))
		{
			sharedImpl->socketTCP = this;
		}

		SocketTCP::~SocketTCP()
		{
			if (isConnected())
				disconnect(DisconnectType::socketDestructor);
			stopAccepting();
			std::lock_guard<std::recursive_mutex> lock1(sharedImpl->mutex);
			se_assert(sharedImpl->socketTCP);
			sharedImpl->socketTCP = nullptr;
		}

		void SocketTCP::waitUntilFinishedReceiving()
		{
			while (isReceiving()) { }
		}

		void SocketTCP::waitUntilFinishedAccepting()
		{
			while (isAccepting()) { }
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			if (sharedImpl->spehsAcceptThread)
			{
				sharedImpl->spehsAcceptThread->join();
				delete sharedImpl->spehsAcceptThread;
				sharedImpl->spehsAcceptThread = nullptr;
			}
		}

		void SocketTCP::waitUntilReceivedHandshake(const time::Time timeout)
		{
			bool wait;
			const time::Time beginTime = time::now();
			do
			{
				if (time::now() - beginTime > timeout)
					return;

				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				wait = !sharedImpl->handshakeReceived;
			} while (wait);
		}

		void SocketTCP::clearReceivedPackets()
		{
			std::lock_guard<std::recursive_mutex> lock2(sharedImpl->receivedPacketsMutex);
			for (size_t i = 0; i < sharedImpl->receivedPackets.size(); i++)
				delete sharedImpl->receivedPackets[i];
			sharedImpl->receivedPackets.clear();
		}

		bool SocketTCP::connect(const Endpoint& endpoint)
		{
			RAIIMutexVariableSetter<bool, std::recursive_mutex> connectingSetter(sharedImpl->connecting, true, sharedImpl->mutex);

			//Stop receiving (outside the main mutex!)
			stopReceiving();

			{
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);

				if (sharedImpl->connected)
				{
					log::info("SocketTCP::connect: failed to connect. Socket is already at the connected state!");
					return false;
				}

				//Start a new connection
				sharedImpl->handshakeSent = false;
				sharedImpl->handshakeReceived = false;

				//Resolve the remote endpoint
				boost::system::error_code error;
				boost::asio::ip::tcp::resolver resolverTCP(sharedImpl->ioService.getImplementationRef());
				const boost::asio::ip::tcp::resolver::query query(endpoint.address.toString(), endpoint.port.toString());
				const boost::asio::ip::tcp::endpoint asioEndpoint = *resolverTCP.resolve(query, error);
				if (error)
				{
					log::info("SocketTCP::connect: failed to resolve the endpoint. Boost asio error: " + error.message());
					return false;
				}
				else
				{
					if (debugLogLevel >= 1)
						log::info("SocketTCP::connect: successfully resolved the remote endpoint.");
				}

				//Connect to the remote boost socket
				try
				{
					sharedImpl->socket.connect(asioEndpoint, error);
				}
				catch (std::exception& exception)
				{
					log::info(std::string("SocketTCP::connect: failed to connect. Exception thrown: ") + exception.what());
					return false;
				}
				if (error)
				{
					log::info("SocketTCP::connect: failed to connect. Boost asio error: " + error.message());
					return false;
				}
				else
				{
					if (debugLogLevel >= 1)
						log::info("SocketTCP::connect: successfully connected the remote boost::asio::ip::tcp::socket.");
				}

				//Expect an incoming handshake after sending one
				startReceiving(sharedImpl->onReceiveCallback);

				//Send the spehs handshake
				WriteBuffer buffer;
				const Handshake handshake;
				buffer.write(handshake);
				sharedImpl->handshakeSent = sendPacket(buffer, PacketType::handshake);
				if (sharedImpl->handshakeSent)
				{
					if (debugLogLevel >= 1)
						log::info("SocketTCP::connect: successfully sent handshake to the remote endpoint.");
				}
				else
				{
					log::info("SocketTCP::connect: failed to connect. Failed to send handshake.");
					return false;//If sending the handshake fails, connection was not successful
				}
			}

			//Wait until received handshake
			waitUntilReceivedHandshake(handshakeReceiveTimeout);
			{
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				if (!sharedImpl->handshakeReceived)
				{
					log::info("SocketTCP::connect: failed to connect! No response handshake received!");
					return false;
				}
			}
			if (debugLogLevel >= 1)
				log::info("SocketTCP::connect: successfully received handshake from the remote endpoint. Socket is now in connected state.");

			//All done, socket is now at connected state!
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			sharedImpl->connected = true;
			sharedImpl->lastReceiveTime = time::now();

			return true;
		}

		void SocketTCP::disconnect()
		{
			disconnect(DisconnectType::unspecified);
		}

		void SocketTCP::disconnect(const DisconnectType disconnectType)
		{
			//Stop receiving (outside the main mutex!)
			stopReceiving();

			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			if (!sharedImpl->connected)
				return;

			if (disconnectType != DisconnectType::doNotSendDisconnectPacket)
			{//Try sending the disconnect packet before disconnecting
				WriteBuffer buffer;
				buffer.write(disconnectType);
				sendPacket(buffer, PacketType::disconnect);
			}

			//Reset the connection state
			sharedImpl->connected = false;
			sharedImpl->handshakeSent = false;
			sharedImpl->handshakeReceived = false;
		}

		void SocketTCP::stopReceiving()
		{
			{
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				if (sharedImpl->socket.is_open())
				{
					try
					{
						sharedImpl->socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
						sharedImpl->socket.close();//TODO: this actually cancels all asynchronous operations, not just receiving...
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

		void SocketTCP::stopAccepting()
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			if (sharedImpl->acceptor)
			{
				try
				{
					sharedImpl->acceptor->close();
				}
				catch (std::exception& e)
				{
					log::info(e.what());
				}
			}
		}

		bool SocketTCP::sendPacket(const WriteBuffer& buffer, const PacketType packetType)
		{
			if (buffer.getOffset() == 0)
			{
				se_assert(false && "Trying to send an empty packet.");
				return true;
			}

			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			boost::system::error_code error;

			if (!sharedImpl->connected && !sharedImpl->connecting && !sharedImpl->accepting)
			{//Can only send user defined packets in the connected state
				log::info("SocketTCP: cannot send a packet. Socket is neither connected, connecting nor accepting.");
				return false;
			}

			//Spehs header
			WriteBuffer headerBuffer;
			const ExpectedBytesType dataBufferSize = ExpectedBytesType(buffer.getOffset());
			const ExpectedBytesType headerBytesValue = ExpectedBytesType(buffer.getOffset() + sizeof(packetType));

			headerBuffer.write(headerBytesValue);
			headerBuffer.write(packetType);

			const size_t headerBufferSize = headerBuffer.getOffset();
			size_t offset = 0;
			while (offset < headerBufferSize)
			{//Keep sending data until the whole header has been sent
				offset += sharedImpl->socket.write_some(boost::asio::buffer(headerBuffer[offset], headerBufferSize - offset), error);
				if (error)
				{//Error occured while sending data...
					log::warning("SocketTCP: failed to send packet's spehs header! Boost asio error: " + error.message());
					return false;
				}
			}

			//Data
			offset = 0;
			while (offset < dataBufferSize)
			{//Keep sending data until all data has been sent
				offset += sharedImpl->socket.write_some(boost::asio::buffer(buffer[offset], dataBufferSize - offset), error);
				if (error)
				{//Error occured while sending data...
					log::warning("SocketTCP: failed to send packet! Boost asio error: " + error.message());
					return false;
				}
			}

			if (debugLogLevel >= 2)
				log::info("SocketTCP: packet sent. Contents: 4(packet byte size) + 1(packet type) + " + std::to_string(buffer.getOffset()) + "(data)");
			return true;
		}

		bool SocketTCP::resizeReceiveBuffer(const size_t newSize)
		{
			if (newSize < sizeof(ExpectedBytesType))
				return false;
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			if (sharedImpl->receiving)
				return false;
			sharedImpl->receiveBuffer.resize(newSize);
			return true;
		}

		bool SocketTCP::startReceiving(const std::function<void(ReadBuffer&)> callbackFunction)
		{
			if (isReceiving())
			{
				log::info("SocketTCP failed to start receiving. Socket is already receiving.");
				return false;
			}

			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			if (!sharedImpl->connected && !sharedImpl->connecting && !sharedImpl->accepting)
			{
				log::warning("SocketTCP: failed to start receiving. Socket is neither connected, connecting nor accepting.");
				return false;
			}
			if (!sharedImpl->socket.is_open())
			{//This should never happen as the connected status should guarantee socket being open...
				log::info("SocketTCP failed to start receiving. Socket has not been opened.");
				return false;
			}

			sharedImpl->receiving = true;
			sharedImpl->lastReceiveTime = time::now();
			sharedImpl->onReceiveCallback = callbackFunction;
			clearReceivedPackets();
			resumeReceiving();

			if (debugLogLevel >= 2)
			{
				if (sharedImpl->expectedBytes > 0)
					log::info("SocketTCP successfully started receiving. Expecting bytes: " + std::to_string(sharedImpl->expectedBytes) + ".");
				else
					log::info("SocketTCP successfully started receiving. Expecting bytes header.");
			}

			return true;
		}

		void SocketTCP::resumeReceiving()
		{
			if (sharedImpl->expectedBytes == 0)
			{//Receive header
				se_assert(sharedImpl->receiveBuffer.size() >= sizeof(sharedImpl->expectedBytes));
				boost::asio::async_read(sharedImpl->socket, boost::asio::buffer(&sharedImpl->receiveBuffer[0], sizeof(sharedImpl->expectedBytes)),
					boost::bind(&SocketTCP::SharedImpl::receiveHandler, sharedImpl->shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
			}
			else
			{//Receive data
				if (sharedImpl->receiveBuffer.size() < sharedImpl->expectedBytes)
				{
					if (debugLogLevel >= 1)
						log::info("SocketTCP: the current receive buffer is insufficient! Expanding to " + std::to_string(sharedImpl->expectedBytes));
					sharedImpl->receiveBuffer.resize(sharedImpl->expectedBytes);
				}
				boost::asio::async_read(sharedImpl->socket, boost::asio::buffer(&sharedImpl->receiveBuffer[0], sharedImpl->expectedBytes),
					boost::bind(&SocketTCP::SharedImpl::receiveHandler, sharedImpl->shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
			}
		}

		void SocketTCP::update()
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);

			//onAccept callback
			if (sharedImpl->onAcceptCallbackQueued)
			{
				sharedImpl->onAcceptCallbackQueued = false;
				sharedImpl->onAcceptCallback(*this);
			}

			//Received packets
			std::lock_guard<std::recursive_mutex> lock2(sharedImpl->receivedPacketsMutex);
			if (sharedImpl->onReceiveCallback)
			{
				for (size_t i = 0; i < sharedImpl->receivedPackets.size(); i++)
				{
					ReadBuffer buffer(sharedImpl->receivedPackets[i]->data(), sharedImpl->receivedPackets[i]->size());//TODO: empty buffer assert?
					sharedImpl->onReceiveCallback(buffer);
				}
			}
			clearReceivedPackets();
		}

		bool SocketTCP::spehsReceiveHandler(ReadBuffer& buffer)
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);

			//Read packet type
			PacketType packetType;
			buffer.read(packetType);
			if (debugLogLevel >= 2)
				log::info("SocketTCP protocol receive handler received packet of type: " + std::to_string((int)packetType) + ", " + std::to_string(buffer.getBytesRemaining()) + " bytes.");

			//Process packet
			switch (packetType)
			{
			case PacketType::undefined:
			{
				const size_t userBytes = buffer.getBytesRemaining();
				if (debugLogLevel >= 2)
					log::info("SocketTCP received user defined packet. Bytes: " + std::to_string(userBytes));
				se_assert(userBytes > 0);

				if (sharedImpl->onReceiveCallback)
				{
					//Push to received packets queue
					std::lock_guard<std::recursive_mutex> lock2(sharedImpl->receivedPacketsMutex);
					sharedImpl->receivedPackets.push_back(new std::vector<uint8_t>(userBytes));
					memcpy(sharedImpl->receivedPackets.back()->data(), buffer[buffer.getOffset()], userBytes);
					return true;
				}
				else
				{
					log::warning("SocketTCP::protocolReceiveHandler: no receive handler provided for the user defined packet.");
					return false;
				}
			}
			case PacketType::disconnect:
			{
				DisconnectType disconnectType;
				buffer.read(disconnectType);
				disconnect(DisconnectType::doNotSendDisconnectPacket);
				log::info("SocketTCP: remote socket gracefully disconnected. Disconnect type was: " + std::to_string((uint8_t)disconnectType));
				return false;
			}
			case PacketType::handshake:
			{
				if (sharedImpl->handshakeReceived)
				{
					log::warning("A handshake for the current connection has already been received!");
					return false;
				}
				Handshake handshake;
				buffer.read(handshake);

				if (handshake.isValid())
				{//VALID HANDSHAKE
					sharedImpl->handshakeReceived = true;
					if (debugLogLevel >= 1)
						log::info("SocketTCP valid handshake received.");
				}
				else
				{//INVALID HANDSHAKE -> DISCARD
					sharedImpl->handshakeReceived = true;
					log::warning("Received an invalid spehs handshake!");
				}
				return false;
			}
			}
			return false;
		}

		bool SocketTCP::startAccepting(const Port& port, const std::function<void(SocketTCP&)> callbackFunction)
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			if (sharedImpl->accepting)
			{
				log::info("SocketTCP failed to start accepting! Socket is already accepting!");
				return false;
			}
			if (sharedImpl->connected)
			{
				log::warning("SocketTCP failed start accepting! Socket is currently connected!");
				return false;
			}
			se_assert(!sharedImpl->handshakeSent);
			se_assert(!sharedImpl->handshakeReceived);
			sharedImpl->onAcceptCallbackQueued = false;

			if (sharedImpl->acceptor == nullptr)
				sharedImpl->acceptor = new boost::asio::ip::tcp::acceptor(sharedImpl->ioService.getImplementationRef());
			boost::system::error_code error;
			const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);

			//Open acceptor
			sharedImpl->acceptor->open(endpoint.protocol(), error);
			if (error)
			{
				log::warning("Failed to open acceptor! Boost asio error: " + error.message());
				return false;
			}

			//Bind acceptor
			sharedImpl->acceptor->bind(endpoint, error);
			if (error)
			{
				log::warning("Failed to bind acceptor! Boost asio error: " + error.message());
				return false;
			}
			if (!sharedImpl->acceptor->is_open())
			{
				log::warning("Boost acceptor failed to open!");
				return false;
			}

			//Make acceptor listen
			sharedImpl->acceptor->listen(boost::asio::socket_base::max_connections, error);
			if (error)
			{
				log::warning("Failed to make acceptor listen! Boost asio error: " + error.message());
				return false;
			}

			//Set the callback function
			sharedImpl->onAcceptCallback = callbackFunction;

			//Start accepting asynchoronously
			sharedImpl->accepting = true;
			sharedImpl->acceptor->async_accept(sharedImpl->socket, boost::bind(&SocketTCP::SharedImpl::onAccept, sharedImpl->shared_from_this(), boost::asio::placeholders::error));

			return true;
		}

		void SocketTCP::spehsAccept()
		{
			//Wait until received remote handshake
			if (debugLogLevel >= 1)
				log::info("Accepting SocketTCP expecting a handshake...");

			//Start expecting an incoming handshake (connector sends first)
			startReceiving(sharedImpl->onReceiveCallback);
			waitUntilReceivedHandshake(handshakeReceiveTimeout);

			{//Check if received the handshake in time
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				if (!sharedImpl->handshakeReceived)
				{
					log::info("SocketTCP failed to accept an incoming connection! No response handshake received!");
					disconnect(DisconnectType::doNotSendDisconnectPacket);
					sharedImpl->onAcceptCallbackQueued = true;
					return;
				}
			}
			if (debugLogLevel >= 1)
				log::info("Accepting SocketTCP received a handshake.");

			//Send a response handshake
			WriteBuffer buffer;
			const Handshake handshake;
			buffer.write(handshake);
			if (sendPacket(buffer, PacketType::handshake))
			{
				sharedImpl->handshakeSent = true;
				if (debugLogLevel >= 1)
					log::info("Accepting SocketTCP sent a handshake.");
			}
			else
			{
				log::info("SocketTCP failed to accept an incoming connection! Could not send handshake!");
				disconnect(DisconnectType::doNotSendDisconnectPacket);

				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				sharedImpl->onAcceptCallbackQueued = true;
				sharedImpl->accepting = false;
				return;
			}

			//Socket is now in the connected state!
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			sharedImpl->accepting = false;
			sharedImpl->connected = true;
			sharedImpl->lastReceiveTime = time::now();
			sharedImpl->onAcceptCallbackQueued = true;
		}

		Address SocketTCP::getRemoteAddress() const
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			if (!isConnected())
			{
				log::info("SocketTCP: cannot retrieve remote endpoint address! Socket is not connected!");
				return "0.0.0.0";
			}
			boost::asio::ip::tcp::endpoint endpoint = sharedImpl->socket.remote_endpoint();
			boost::asio::ip::address address = endpoint.address();
			boost::asio::ip::address_v4 address_v4 = address.to_v4();
			return address_v4.to_string();
		}

		Port SocketTCP::getRemotePort() const
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			if (!isConnected())
			{
				log::info("SocketTCP: cannot retrieve remote endpoint port! Socket is not connected!");
				return 0;
			}
			boost::asio::ip::tcp::endpoint endpoint = sharedImpl->socket.remote_endpoint();
			return endpoint.port();
		}

		Endpoint SocketTCP::getRemoteEndpoint() const
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			if (!isConnected())
			{
				log::info("SocketTCP: cannot retrieve remote endpoint port! Socket is not connected!");
				return Endpoint::invalid;
			}
			return Endpoint(sharedImpl->socket.remote_endpoint().address().to_v4().to_string(), sharedImpl->socket.remote_endpoint().port());
		}

		bool SocketTCP::isAccepting() const
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			return sharedImpl->accepting;
		}

		bool SocketTCP::isReceiving() const
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			return sharedImpl->receiving;
		}

		bool SocketTCP::isConnected() const
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			if (time::now() - sharedImpl->lastReceiveTime >= connectionTimeout)
				return false;
			return sharedImpl->connected;
		}



		SocketTCP::SharedImpl::SharedImpl(IOService& _ioService)
			: ioService(_ioService)
			, socket(_ioService.getImplementationRef())
			, receiveBuffer(512)
		{

		}

		SocketTCP::SharedImpl::~SharedImpl()
		{
			if (acceptor)
			{
				delete acceptor;
				acceptor = nullptr;
			}
			se_assert(spehsAcceptThread == nullptr);
		}

		void SocketTCP::SharedImpl::receiveHandler(const boost::system::error_code& error, std::size_t bytes)
		{
			if (debugLogLevel >= 2)
				log::info("SocketTCP receive handler received " + std::to_string(bytes) + " bytes.");

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
					log::info("SocketTCP disconnected. Remote socket closed connection.");
					if (socketTCP)
						socketTCP->disconnect(DisconnectType::doNotSendDisconnectPacket);
					return;
				}
				else if (error == boost::asio::error::connection_reset)
				{//Disconnect
					log::info("SocketTCP disconnected. Remote socket closed connection.");
					if (socketTCP)
						socketTCP->disconnect(DisconnectType::doNotSendDisconnectPacket);
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
					if (socketTCP)
						socketTCP->disconnect(DisconnectType::doNotSendDisconnectPacket);
					return;
				}
				else
				{//Ignored error
					log::info("Ignored ASIO error: " + std::to_string(error.value()) + ": " + error.message());
				}
			}

			if (bytes)
			{
				ReadBuffer readBuffer(&receiveBuffer[0], bytes);
				if (expectedBytes == 0)
				{//Header received
					readBuffer.read(expectedBytes);
					if (socketTCP)
						socketTCP->resumeReceiving();
				}
				else if (expectedBytes == bytes)
				{//Data received

				 //Read buffer
					const bool keepReceiving = socketTCP ? socketTCP->spehsReceiveHandler(readBuffer) : false;
					expectedBytes = 0;//Begin to expect header next
					if (keepReceiving)
						socketTCP->resumeReceiving();
				}
				else
				{
					log::warning("SocketTCP received unexpected amount of bytes! Canceling receiving!");
				}
			}
		}

		void SocketTCP::SharedImpl::onAccept(const boost::system::error_code error)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (socketTCP == nullptr)
				return;
			se_assert(!socketTCP->isConnected());
			se_assert(!socketTCP->isReceiving());
			se_assert(socketTCP->isAccepting());
			se_assert(acceptor);
			se_assert(!spehsAcceptThread);
			acceptor->close();
			if (error)
			{
				log::warning("SocketTCP failed to accept an incoming connection! Boost asio error: " + error.message() + "Accepting has stopped.");
				socketTCP->disconnect(DisconnectType::doNotSendDisconnectPacket);
				accepting = false;
				return;
			}
			else
			{
				spehsAcceptThread = new std::thread(&SocketTCP::spehsAccept, socketTCP);
			}
		}
	}
}