#include "stdafx.h"
#include "SpehsEngine/Net/SocketTCP.h"

#include <assert.h>
#include <iostream>
#include <string>
#include "SpehsEngine/Net/Acceptor.h"
#include "SpehsEngine/Net/IOService.h"
#include <SpehsEngine/Core/WriteBuffer.h>
#include <SpehsEngine/Core/ReadBuffer.h>
#include "SpehsEngine/Core/RAIIVariableSetter.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"

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
	extern std::string workingDirectory;
	namespace net
	{
		std::string toString(const boost::asio::ip::tcp::endpoint& endpoint)
		{
			return endpoint.address().to_string() + ":" + std::to_string(endpoint.port());
		}

		class Handshake
		{
			static const uint16_t endiannessCheckBytes = 0xACDC;
		public:

			bool isValid() const
			{
				return valid;
			}

			void write(WriteBuffer& buffer) const
			{
				buffer.write(currentMagic);
				buffer.write(handshakeVersion);
				buffer.write(endiannessCheckBytes);
			}

			bool read(ReadBuffer& buffer)
			{
				//NOTE: buffer can contain invalid data! If so, set the valid boolean to false
				valid = true;

				//Magic
				if (!buffer.read(magic))
				{
					log::info("Handshake::read() invalid handshake. No bytes left to read magic.");
					valid = false;
					return false;
				}

				if (magic != currentMagic)
				{
					log::info("Handshake::read() invalid handshake. Incompatible magic - my version: " + se::toHexString(currentMagic) + ", read magic: " + se::toHexString(magic));
					valid = false;
					return false;
				}

				//Handshake version
				if (!buffer.read(handshakeVersion))
				{
					log::info("Handshake::read() invalid handshake. No bytes left to read handshake version.");
					valid = false;
					return false;
				}

				if (handshakeVersion != currentVersion)
				{
					log::info("Handshake::read() invalid handshake. Incompatible versions - my version: " + std::to_string(currentVersion) + ", other version: " + std::to_string(handshakeVersion));
					valid = false;
					return false;
				}

				//Endianness check bytes
				uint16_t readEndiannessCheckBytes;
				if (!buffer.read(readEndiannessCheckBytes))
				{
					log::info("Handshake::read() invalid handshake. No bytes left to read endianness check bytes.");
					valid = false;
					return false;
				}

				if (readEndiannessCheckBytes != endiannessCheckBytes)
				{
					log::info("Handshake::read() invalid handshake. Invalid endianness check bytes.");
					valid = false;
					return false;
				}

				return true;
			}

		private:
			typedef uint16_t VersionType;
			static const VersionType currentVersion = 1;
			static const uint64_t currentMagic = 0xC0DEC5F070C01001;//Mmmm...magic
			uint64_t magic = currentMagic;
			VersionType handshakeVersion = currentVersion;
			bool valid = true;
		};

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
			close();
			std::lock_guard<std::recursive_mutex> lock1(sharedImpl->mutex);
			se_assert(sharedImpl->socketTCP);
			sharedImpl->socketTCP = nullptr;
		}

		bool SocketTCP::open()
		{
			return sharedImpl->open();
		}

		void SocketTCP::close()
		{
			sharedImpl->disconnect();
			sharedImpl->close();
		}

		bool SocketTCP::bind(const Port& port)
		{
			return sharedImpl->bind(port);
		}

		bool SocketTCP::isOpen() const
		{
			return sharedImpl->isOpen();
		}

		void SocketTCP::waitUntilFinishedReceiving()
		{
			while (isReceiving()) { }
		}

		void SocketTCP::waitUntilFinishedAccepting()
		{
			while (getAcceptingState() != AcceptingState::idle) { }
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

		size_t SocketTCP::getSentBytes() const
		{
			return sharedImpl->getSentBytes();
		}

		size_t SocketTCP::getReceivedBytes() const
		{
			return sharedImpl->getReceivedBytes();
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

		void SocketTCP::setNoDelay(const bool enabled)
		{
			sharedImpl->setNoDelay(enabled);
		}

		bool SocketTCP::getNoDelay() const
		{
			return sharedImpl->getNoDelay();
		}

		void SocketTCP::setReuseAddress(const bool enabled)
		{
			sharedImpl->setReuseAddress(enabled);
		}

		bool SocketTCP::getReuseAddress() const
		{
			return sharedImpl->getReuseAddress();
		}

		void SocketTCP::setKeepAlive(const bool enabled)
		{
			sharedImpl->setKeepAlive(enabled);
		}

		bool SocketTCP::getKeepAlive() const
		{
			return sharedImpl->getKeepAlive();
		}

		Address SocketTCP::getRemoteAddress() const
		{
			return sharedImpl->getRemoteAddress();
		}

		Port SocketTCP::getRemotePort() const
		{
			return sharedImpl->getRemotePort();
		}

		boost::asio::ip::tcp::endpoint SocketTCP::getRemoteEndpoint() const
		{
			return sharedImpl->getRemoteEndpoint();
		}

		Port SocketTCP::getLocalPort() const
		{
			return sharedImpl->getLocalPort();
		}

		boost::asio::ip::tcp::endpoint SocketTCP::getLocalEndpoint() const
		{
			return sharedImpl->getLocalEndpoint();
		}

		SocketTCP::AcceptingState SocketTCP::getAcceptingState() const
		{
			return sharedImpl->getAcceptingState();
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

		void SocketTCP::setDebugLogLevel(const int level)
		{
			sharedImpl->setDebugLogLevel(level);
		}

		int SocketTCP::getDebugLogLevel() const
		{
			return sharedImpl->getDebugLogLevel();
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

		bool SocketTCP::SharedImpl::open()
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

		void SocketTCP::SharedImpl::close()
		{
			{
				std::lock_guard<std::recursive_mutex> lock(mutex);
				if (socket.is_open())
				{
					onReceiveCallback = std::function<void(ReadBuffer&)>();
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
						se::log::error("Received packets will be cleared. Should this really happen?");
					}
					clearReceivedPackets();
				}
			}
			while (isReceiving())
			{
				//Blocks
			}
		}

		bool SocketTCP::SharedImpl::bind(const Port& port)
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

		bool SocketTCP::SharedImpl::isOpen() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return socket.is_open();
		}

		Port SocketTCP::SharedImpl::getLocalPort() const
		{
			return getLocalEndpoint().port();
		}

		boost::asio::ip::tcp::endpoint SocketTCP::SharedImpl::getLocalEndpoint() const
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
			return localEndpoint;
		}

		Address SocketTCP::SharedImpl::getRemoteAddress() const
		{
			return Address(getRemoteEndpoint().address().to_string());
		}

		Port SocketTCP::SharedImpl::getRemotePort() const
		{
			return Port(getRemoteEndpoint().port());
		}

		boost::asio::ip::tcp::endpoint SocketTCP::SharedImpl::getRemoteEndpoint() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			boost::system::error_code error;
			boost::asio::ip::tcp::endpoint remoteEndpoint = socket.remote_endpoint(error);
			if (error)
			{
				log::error("SocketTCP: failed to get remote endpoint. Boost asio error: " + error.message());
			}
			return remoteEndpoint;
		}

		void SocketTCP::SharedImpl::receiveHandler(const boost::system::error_code& error, std::size_t bytes)
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
					//Connection gracefully closed
					DEBUG_LOG(1, "disconnected. Remote socket closed connection.");
					if (socketTCP)
					{
						socketTCP->disconnect(DisconnectType::doNotSendDisconnectPacket);
					}
					return;
				}
				else if (error == boost::asio::error::connection_reset)
				{
					//Disconnect
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
				{//Ignored error
					DEBUG_LOG(1, "error while receiving. Ignored boost asio error: " + std::to_string(error.value()) + ": " + error.message());
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
					if (debugLogLevel >= 1)
					{
						log::error("SocketTCP received unexpected amount of bytes! Canceling receiving!");
					}
				}
			}
		}

		bool SocketTCP::SharedImpl::startAccepting(const Port& port, const std::function<void(SocketTCP&)> callbackFunction)
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
				acceptor.reset(new boost::asio::ip::tcp::acceptor(ioService.getImplementationRef()));
			boost::system::error_code error;
			const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);

			//Open acceptor
			acceptor->open(endpoint.protocol(), error);
			if (error)
			{
				DEBUG_LOG(1, "failed to open acceptor! Boost asio error: " + error.message());
				return false;
			}

			//Bind acceptor
			acceptor->bind(endpoint, error);
			if (error)
			{
				DEBUG_LOG(1, "failed to bind acceptor! Boost asio error: " + error.message());
				return false;
			}

			//Make acceptor listen
			acceptor->listen(boost::asio::socket_base::max_connections, error);
			if (error)
			{
				DEBUG_LOG(1, "failed to make acceptor listen! Boost asio error: " + error.message());
				return false;
			}

			//Set the callback function
			onAcceptCallback = callbackFunction;

			//Start accepting asynchoronously
			acceptingState = AcceptingState::listeningForConnection;
			acceptor->async_accept(socket, boost::bind(&SocketTCP::SharedImpl::onAccept, shared_from_this(), boost::asio::placeholders::error));

			return true;
		}

		void SocketTCP::SharedImpl::onAccept(const boost::system::error_code error)
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
				spehsAcceptThread.reset(new std::thread(&SocketTCP::SharedImpl::spehsAccept, this));
			}
		}

		void SocketTCP::SharedImpl::spehsAccept()
		{
			//Start expecting an incoming handshake (connector sends first)
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

			//NOTE: no mutex locking during the wait time
			waitForHandshake(handshakeReceiveTimeout);

			//Lock the mutex
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (!socketTCP)
			{
				return;
			}

			//Check if received the handshake in time
			if (!handshakeReceived)
			{
				DEBUG_LOG(1, "failed to accept an incoming connection! No response handshake received!");
				socketTCP->disconnect(DisconnectType::doNotSendDisconnectPacket);
				onAcceptCallbackQueued = true;
				return;
			}

			DEBUG_LOG(1, "accepting socket received a handshake.");

			//Send a response handshake
			WriteBuffer buffer;
			const Handshake handshake;
			buffer.write(handshake);
			if (socketTCP->sendPacket(buffer, PacketType::handshake))
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

			//Socket is now in the connected state!
			acceptingState = AcceptingState::idle;
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
				update();
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
				log::error("SocketTCP is already receiving.");
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
					DEBUG_LOG(1, "the current receive buffer is insufficient! Expanding to " + std::to_string(expectedBytes));
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
				boost::system::error_code error;
				acceptor->close(error);
				if (error)
				{
					DEBUG_LOG(1, "failed to stop accepting. Boost asio error: " + error.message());
				}
			}
		}

		bool SocketTCP::SharedImpl::connect(const Endpoint& endpoint)
		{
			if (isConnected())
			{
				DEBUG_LOG(1, "failed to connect. Socket is already connected.");
				return false;
			}

			RAIIMutexVariableSetter<bool, std::recursive_mutex> connectingSetter(connecting, true, mutex);

			{
				std::lock_guard<std::recursive_mutex> lock(mutex);

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
					DEBUG_LOG(1, "failed to resolve the endpoint. Boost asio error: " + error.message());
					return false;
				}
				else
				{
					DEBUG_LOG(1, "successfully resolved the remote endpoint.");
				}

				//Connect to the remote boost socket
				socket.connect(asioEndpoint, error);
				if (error)
				{
					DEBUG_LOG(1, "failed to connect(). Boost asio error: " + error.message());
					return false;
				}
				else
				{
					DEBUG_LOG(1, "successfully connected the remote boost socket.");
				}

				//Expect an incoming handshake after sending one
				startReceiving();

				//Send the spehs handshake
				WriteBuffer buffer;
				const Handshake handshake;
				buffer.write(handshake);
				const time::Time handshakeSendBeginTime = time::now();
				while (time::now() - handshakeSendBeginTime < se::time::fromSeconds(5.0f))
				{
					if (sendPacket(buffer, PacketType::handshake))
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
					return false;//If sending the handshake fails, connection was not successful
				}
			}

			//Wait until received handshake
			waitForHandshake(handshakeReceiveTimeout);
			{
				std::lock_guard<std::recursive_mutex> lock(mutex);
				if (!handshakeReceived)
				{
					DEBUG_LOG(1, "failed to connect! No response handshake received!");
					return false;
				}
			}

			//All done, socket is now at connected state!
			DEBUG_LOG(1, "successfully received handshake from the remote endpoint. Socket is now in connected state.");
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
			{
				return;
			}

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

		bool SocketTCP::SharedImpl::sendPacket(const WriteBuffer& buffer, const PacketType packetType)
		{
			if (buffer.getSize() == 0)
			{
				se_assert(false && "Trying to send an empty packet.");
				return true;
			}

			std::lock_guard<std::recursive_mutex> lock(mutex);
			boost::system::error_code error;

			if (!connected && !connecting && acceptingState == AcceptingState::idle)
			{//Can only send user defined packets in the connected state
				DEBUG_LOG(1, "cannot send a packet. Socket is neither connected, connecting nor accepting.");
				return false;
			}

			//Spehs header
			WriteBuffer headerBuffer;
			const ExpectedBytesType dataBufferSize = ExpectedBytesType(buffer.getSize());
			const ExpectedBytesType headerBytesValue = ExpectedBytesType(buffer.getSize() + sizeof(packetType));

			headerBuffer.write(headerBytesValue);
			headerBuffer.write(packetType);

			const size_t headerBufferSize = headerBuffer.getSize();
			size_t offset = 0;
			while (offset < headerBufferSize)
			{//Keep sending data until the whole header has been sent
				const size_t writtenBytes = socket.write_some(boost::asio::buffer(headerBuffer[offset], headerBufferSize - offset), error);
				offset += writtenBytes;
				sentBytes += writtenBytes;
				if (error)
				{//Error occured while sending data...
					DEBUG_LOG(0, "failed to send packet's spehs header! Boost asio error: " + error.message());
					return false;
				}
			}

			//Data
			offset = 0;
			while (offset < dataBufferSize)
			{//Keep sending data until all data has been sent
				const size_t writtenBytes = socket.write_some(boost::asio::buffer(buffer[offset], dataBufferSize - offset), error);
				offset += writtenBytes;
				sentBytes += writtenBytes;
				if (error)
				{//Error occured while sending data...
					DEBUG_LOG(0, "SocketTCP: failed to send packet! Boost asio error: " + error.message());
					return false;
				}
			}

			DEBUG_LOG(2, "sent 4(packet byte size) + 1(packet type) + " + std::to_string(buffer.getSize()) +
				" bytes to: " + getLocalEndpoint().address().to_string() + ":" + std::to_string(getLocalEndpoint().port()));

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
			{
				//onAccept callback
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
				std::function<void(ReadBuffer&)> handler;
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
					if (receivedPackets.empty() || !onReceiveCallback)
					{
						break;
					}
					else
					{
						//Copy contents to non-mutex protected memory
						data.swap(receivedPackets.front().get()->data);
						handler = onReceiveCallback;
						receivedPackets.erase(receivedPackets.begin());
					}
				}
				ReadBuffer readBuffer(data.data(), data.size());
				handler(readBuffer);
			}
		}

		bool SocketTCP::SharedImpl::spehsReceiveHandler(ReadBuffer& buffer)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);

			//Read packet type
			PacketType packetType;
			buffer.read(packetType);
			DEBUG_LOG(2, "received packet of type: " + std::to_string((int)packetType) + ", " + std::to_string(buffer.getBytesRemaining()) + " bytes.");

			//Process packet
			switch (packetType)
			{
			default:
				break;
			case PacketType::undefined:
			{
				const size_t userBytes = buffer.getBytesRemaining();
				DEBUG_LOG(2, "received user defined packet. Bytes: " + std::to_string(userBytes));
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
				buffer.read(handshake);

				if (handshake.isValid())
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

		void SocketTCP::SharedImpl::setNoDelay(const bool enabled)
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

		bool SocketTCP::SharedImpl::getNoDelay() const
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

		void SocketTCP::SharedImpl::setReuseAddress(const bool enabled)
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

		bool SocketTCP::SharedImpl::getReuseAddress() const
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

		void SocketTCP::SharedImpl::setKeepAlive(const bool enabled)
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

		bool SocketTCP::SharedImpl::getKeepAlive() const
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

		SocketTCP::AcceptingState SocketTCP::SharedImpl::getAcceptingState() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return acceptingState;
		}

		bool SocketTCP::SharedImpl::isAccepting() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return acceptingState != SocketTCP::AcceptingState::idle;
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

		size_t SocketTCP::SharedImpl::getSentBytes() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return sentBytes;
		}

		size_t SocketTCP::SharedImpl::getReceivedBytes() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return receivedBytes;
		}

		void SocketTCP::SharedImpl::setDebugLogLevel(const int level)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			debugLogLevel = level;
		}

		int SocketTCP::SharedImpl::getDebugLogLevel() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return debugLogLevel;
		}
	}
}
