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
		}

		bool SocketTCP::waitForHandshake(const time::Time timeout)
		{
			return sharedImpl->waitForHandshake(timeout);
		}

		void SocketTCP::clearReceivedPackets()
		{
			sharedImpl->clearReceivedPackets();
		}

		bool SocketTCP::connect(const Endpoint& endpoint)
		{
			return sharedImpl->connect(endpoint);
		}

		void SocketTCP::disconnect()
		{
			sharedImpl->disconnect(DisconnectType::unspecified);
		}

		void SocketTCP::disconnect(const DisconnectType disconnectType)
		{
			sharedImpl->disconnect(disconnectType);
		}

		void SocketTCP::stopAccepting()
		{
			sharedImpl->stopAccepting();
		}

		bool SocketTCP::sendPacket(const WriteBuffer& buffer, const PacketType packetType)
		{
			return sharedImpl->sendPacket(buffer, packetType);
		}

		bool SocketTCP::resizeReceiveBuffer(const size_t newSize)
		{
			return sharedImpl->resizeReceiveBuffer(newSize);
		}

		void SocketTCP::setOnReceiveCallback(const std::function<void(ReadBuffer&)> callbackFunction)
		{
			sharedImpl->setOnReceiveCallback(callbackFunction);
		}
		
		void SocketTCP::startReceiving()
		{
			sharedImpl->startReceiving();
		}

		void SocketTCP::update()
		{
			return sharedImpl->update();
		}

		bool SocketTCP::spehsReceiveHandler(ReadBuffer& buffer)
		{
			return sharedImpl->spehsReceiveHandler(buffer);
		}

		bool SocketTCP::startAccepting(const Port& port, const std::function<void(SocketTCP&)> callbackFunction)
		{
			return sharedImpl->startAccepting(port, callbackFunction);
		}

		Address SocketTCP::getRemoteAddress() const
		{
			return sharedImpl->getRemoteAddress();
		}

		Port SocketTCP::getRemotePort() const
		{
			return sharedImpl->getRemotePort();
		}

		Endpoint SocketTCP::getRemoteEndpoint() const
		{
			return sharedImpl->getRemoteEndpoint();
		}

		bool SocketTCP::isAccepting() const
		{
			return sharedImpl->isAccepting();
		}

		bool SocketTCP::isReceiving() const
		{
			return sharedImpl->isReceiving();
		}

		bool SocketTCP::isConnected() const
		{
			return sharedImpl->isConnected();
		}



		SocketTCP::SharedImpl::SharedImpl(IOService& _ioService)
			: ioService(_ioService)
			, socket(_ioService.getImplementationRef())
			, receiveBuffer(512)
		{

		}

		SocketTCP::SharedImpl::~SharedImpl()
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
					startReceiving();
				}
				else if (expectedBytes == bytes)
				{//Data received

				 //Read buffer
					const bool keepReceiving = socketTCP ? socketTCP->spehsReceiveHandler(readBuffer) : false;
					expectedBytes = 0;//Begin to expect header next
					if (keepReceiving)
						startReceiving();
				}
				else
				{
					log::error("SocketTCP received unexpected amount of bytes! Canceling receiving!");
				}
			}
		}

		bool SocketTCP::SharedImpl::startAccepting(const Port& port, const std::function<void(SocketTCP&)> callbackFunction)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (accepting)
			{
				log::info("SocketTCP failed to start accepting! Socket is already accepting!");
				return false;
			}
			if (connected)
			{
				log::warning("SocketTCP failed start accepting! Socket is currently connected!");
				return false;
			}
			se_assert(!handshakeSent);
			se_assert(!handshakeReceived);
			onAcceptCallbackQueued = false;

			if (!acceptor)
				acceptor.reset(new boost::asio::ip::tcp::acceptor(ioService.getImplementationRef()));
			boost::system::error_code error;
			const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);

			//Open acceptor
			acceptor->open(endpoint.protocol(), error);
			if (error)
			{
				log::warning("Failed to open acceptor! Boost asio error: " + error.message());
				return false;
			}

			//Bind acceptor
			acceptor->bind(endpoint, error);
			if (error)
			{
				log::warning("Failed to bind acceptor! Boost asio error: " + error.message());
				return false;
			}
			if (!acceptor->is_open())
			{
				log::warning("Boost acceptor failed to open!");
				return false;
			}

			//Make acceptor listen
			acceptor->listen(boost::asio::socket_base::max_connections, error);
			if (error)
			{
				log::warning("Failed to make acceptor listen! Boost asio error: " + error.message());
				return false;
			}

			//Set the callback function
			onAcceptCallback = callbackFunction;

			//Start accepting asynchoronously
			accepting = true;
			acceptor->async_accept(socket, boost::bind(&SocketTCP::SharedImpl::onAccept, shared_from_this(), boost::asio::placeholders::error));

			return true;
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
			acceptor->close();
			if (error)
			{
				log::warning("SocketTCP failed to accept an incoming connection! Boost asio error: " + error.message() + "Accepting has stopped.");
				socketTCP->disconnect(DisconnectType::doNotSendDisconnectPacket);
				accepting = false;
				return;
			}

			spehsAcceptThread.reset(new std::thread(&SocketTCP::SharedImpl::spehsAccept, this));
		}

		void SocketTCP::SharedImpl::spehsAccept()
		{
			//Start expecting an incoming handshake (connector sends first)
			{
				std::lock_guard<std::recursive_mutex> lock(mutex);
				if (!socketTCP)
					return;
				if (debugLogLevel >= 1)
					log::info("Accepting SocketTCP expecting a handshake...");
				startReceiving();
			}

			//NOTE: no mutex locking during the wait time
			waitForHandshake(handshakeReceiveTimeout);

			//Lock the mutex
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (!socketTCP)
				return;

			//Check if received the handshake in time
			if (!handshakeReceived)
			{
				log::info("SocketTCP failed to accept an incoming connection! No response handshake received!");
				socketTCP->disconnect(DisconnectType::doNotSendDisconnectPacket);
				onAcceptCallbackQueued = true;
				return;
			}

			if (debugLogLevel >= 1)
				log::info("Accepting SocketTCP received a handshake.");

			//Send a response handshake
			WriteBuffer buffer;
			const Handshake handshake;
			buffer.write(handshake);
			if (socketTCP->sendPacket(buffer, PacketType::handshake))
			{
				if (debugLogLevel >= 1)
					log::info("Accepting SocketTCP sent a handshake.");
				handshakeSent = true;
			}
			else
			{
				log::info("SocketTCP failed to accept an incoming connection! Could not send handshake!");
				socketTCP->disconnect(DisconnectType::doNotSendDisconnectPacket);
				onAcceptCallbackQueued = true;
				accepting = false;
				return;
			}

			//Socket is now in the connected state!
			accepting = false;
			connected = true;
			lastReceiveTime = time::now();
			onAcceptCallbackQueued = true;
		}

		bool SocketTCP::SharedImpl::waitForHandshake(const time::Time timeout)
		{
			const time::Time beginTime = time::now();
			while (true)
			{
				if (time::now() - beginTime > timeout)
					return false;
				std::lock_guard<std::recursive_mutex> lock(mutex);
				if (handshakeReceived)
					return true;
				if (destructorCalled)
					return false;
			}
		}

		void SocketTCP::SharedImpl::startReceiving()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (receiving)
			{
				se_assert(false && "Already receiving");
				return;
			}
			if (expectedBytes == 0)
			{//Receive header
				se_assert(receiveBuffer.size() >= sizeof(expectedBytes));
				boost::asio::async_read(socket, boost::asio::buffer(&receiveBuffer[0], sizeof(expectedBytes)),
					boost::bind(&SocketTCP::SharedImpl::receiveHandler, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
			}
			else
			{//Receive data
				if (receiveBuffer.size() < expectedBytes)
				{
					if (debugLogLevel >= 1)
						log::info("SocketTCP: the current receive buffer is insufficient! Expanding to " + std::to_string(expectedBytes));
					receiveBuffer.resize(expectedBytes);
				}
				boost::asio::async_read(socket, boost::asio::buffer(&receiveBuffer[0], expectedBytes),
					boost::bind(&SocketTCP::SharedImpl::receiveHandler, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
			}
			receiving = true;
		}

		void SocketTCP::SharedImpl::clearReceivedPackets()
		{
			std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
			receivedPackets.clear();
		}

		void SocketTCP::SharedImpl::stopAccepting()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (acceptor)
			{
				try
				{
					acceptor->close();
				}
				catch (std::exception& e)
				{
					log::info(e.what());
				}
			}
		}

		bool SocketTCP::SharedImpl::connect(const Endpoint& endpoint)
		{
			if (isConnected())
				disconnect();

			RAIIMutexVariableSetter<bool, std::recursive_mutex> connectingSetter(connecting, true, mutex);

			{
				std::lock_guard<std::recursive_mutex> lock(mutex);

				if (connected)
				{
					log::info("SocketTCP::connect: failed to connect. Socket is already at the connected state!");
					return false;
				}

				//Start a new connection
				expectedBytes = 0;
				handshakeSent = false;
				handshakeReceived = false;

				//Resolve the remote endpoint
				boost::system::error_code error;
				boost::asio::ip::tcp::resolver resolverTCP(ioService.getImplementationRef());
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
					socket.connect(asioEndpoint, error);
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
				startReceiving();

				//Send the spehs handshake
				WriteBuffer buffer;
				const Handshake handshake;
				buffer.write(handshake);
				time::delay(time::fromSeconds(3.0f));
				handshakeSent = sendPacket(buffer, PacketType::handshake);
				if (handshakeSent)
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
			waitForHandshake(handshakeReceiveTimeout);
			{
				std::lock_guard<std::recursive_mutex> lock(mutex);
				if (!handshakeReceived)
				{
					log::info("SocketTCP::connect: failed to connect! No response handshake received!");
					return false;
				}
			}
			if (debugLogLevel >= 1)
				log::info("SocketTCP::connect: successfully received handshake from the remote endpoint. Socket is now in connected state.");

			//All done, socket is now at connected state!
			std::lock_guard<std::recursive_mutex> lock(mutex);
			connected = true;

			return true;
		}

		void SocketTCP::SharedImpl::disconnect()
		{
			disconnect(DisconnectType::unspecified);
		}

		void SocketTCP::SharedImpl::disconnect(const DisconnectType disconnectType)
		{
			if (!isConnected())
				return;

			if (disconnectType != DisconnectType::doNotSendDisconnectPacket)
			{//Try sending the disconnect packet before disconnecting
				WriteBuffer buffer;
				buffer.write(disconnectType);
				sendPacket(buffer, PacketType::disconnect);
			}

			//Reset the connection state
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
			connected = false;
			handshakeSent = false;
			handshakeReceived = false;
		}

		bool SocketTCP::SharedImpl::sendPacket(const WriteBuffer& buffer, const PacketType packetType)
		{
			if (buffer.getOffset() == 0)
			{
				se_assert(false && "Trying to send an empty packet.");
				return true;
			}

			std::lock_guard<std::recursive_mutex> lock(mutex);
			boost::system::error_code error;

			if (!connected && !connecting && !accepting)
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
				offset += socket.write_some(boost::asio::buffer(headerBuffer[offset], headerBufferSize - offset), error);
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
				offset += socket.write_some(boost::asio::buffer(buffer[offset], dataBufferSize - offset), error);
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

		bool SocketTCP::SharedImpl::resizeReceiveBuffer(const size_t newSize)
		{
			if (newSize < sizeof(ExpectedBytesType))
				return false;
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (receiving)
				return false;
			receiveBuffer.resize(newSize);
			return true;
		}

		void SocketTCP::SharedImpl::setOnReceiveCallback(const std::function<void(ReadBuffer&)> callbackFunction)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			onReceiveCallback = callbackFunction;
		}

		void SocketTCP::SharedImpl::update()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);

			//onAccept callback
			if (onAcceptCallbackQueued)
			{
				onAcceptCallbackQueued = false;
				if (socketTCP)
					onAcceptCallback(*socketTCP);
			}

			//Received packets
			std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
			if (onReceiveCallback)
			{
				for (size_t i = 0; i < receivedPackets.size(); i++)
				{
					ReadBuffer buffer(receivedPackets[i]->data.data(), receivedPackets[i]->data.size());//TODO: empty buffer assert?
					onReceiveCallback(buffer);
					if (!onReceiveCallback)
					{
						receivedPackets.erase(receivedPackets.begin() + i);
						break;
					}
				}
				if (onReceiveCallback)
					clearReceivedPackets();
			}
		}

		bool SocketTCP::SharedImpl::spehsReceiveHandler(ReadBuffer& buffer)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);

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

				//Push to received packets queue
				std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
				receivedPackets.push_back(std::unique_ptr<ReceivedPacket>(new ReceivedPacket(userBytes)));
				memcpy(receivedPackets.back()->data.data(), buffer[buffer.getOffset()], userBytes);
				return true;
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
				if (handshakeReceived)
				{
					log::warning("A handshake for the current connection has already been received!");
					return false;
				}
				Handshake handshake;
				buffer.read(handshake);

				if (handshake.isValid())
				{//VALID HANDSHAKE
					if (debugLogLevel >= 1)
						log::info("SocketTCP valid handshake received.");
				}
				else
				{//INVALID HANDSHAKE -> DISCARD
					log::warning("Received an invalid spehs handshake!");
				}
				handshakeReceived = true;
				return true;
			}
			}
			return false;
		}

		Address SocketTCP::SharedImpl::getRemoteAddress() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (!isConnected())
			{
				log::info("SocketTCP: cannot retrieve remote endpoint address! Socket is not connected!");
				return "0.0.0.0";
			}
			boost::asio::ip::tcp::endpoint endpoint = socket.remote_endpoint();
			boost::asio::ip::address address = endpoint.address();
			boost::asio::ip::address_v4 address_v4 = address.to_v4();
			return address_v4.to_string();
		}

		Port SocketTCP::SharedImpl::getRemotePort() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (!isConnected())
			{
				log::info("SocketTCP: cannot retrieve remote endpoint port! Socket is not connected!");
				return 0;
			}
			boost::asio::ip::tcp::endpoint endpoint = socket.remote_endpoint();
			return endpoint.port();
		}

		Endpoint SocketTCP::SharedImpl::getRemoteEndpoint() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (!isConnected())
			{
				log::info("SocketTCP: cannot retrieve remote endpoint port! Socket is not connected!");
				return Endpoint::invalid;
			}
			return Endpoint(socket.remote_endpoint().address().to_v4().to_string(), socket.remote_endpoint().port());
		}

		bool SocketTCP::SharedImpl::isAccepting() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return accepting;
		}

		bool SocketTCP::SharedImpl::isReceiving() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return receiving;
		}

		bool SocketTCP::SharedImpl::isConnected() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (time::now() - lastReceiveTime >= connectionTimeout)
				return false;
			return connected;
		}
	}
}
