#include <assert.h>
#include <iostream>
#include <string>
#include "SpehsEngine/Net/SocketTCP.h"
#include "SpehsEngine/Net/Acceptor.h"
#include "SpehsEngine/Net/IOService.h"
#include "SpehsEngine/Net/Handshake.h"
#include <SpehsEngine/Core/WriteBuffer.h>
#include <SpehsEngine/Core/ReadBuffer.h>
#include "SpehsEngine/Core/RAIIVariableSetter.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Core/StringOperations.h"

namespace se
{
	extern std::string workingDirectory;
	/*
		level 1: prints most essential state changes.
		level 2: prints some network traffic numbers.
		level 3: prints receive buffer in hex string.
	*/
	int debugLogLevel = 0;
	static const se::time::Time handshakeReceiveTimeout = se::time::fromSeconds(10000);
	static const se::time::Time connectionTimeout = se::time::fromSeconds(10000);

	SocketTCP::Id SocketTCP::nextId = 0;
	SocketTCP::SocketTCP(IOService& _ioService)
		: id(nextId++)
		, ioService(_ioService)
		, socket(_ioService.getImplementationRef())
		, acceptor(nullptr)
		, spehsAcceptThread(nullptr)
		, onReceiveCallback()
		, onAcceptCallback()
		, expectedBytes(0)
		, receiveBuffer(512)
		, lastReceiveTime(0)
		, receiving(false)
		, accepting(false)
		, connected(false)
		, connecting(false)
		, handshakeSent(false)
		, handshakeReceived(false)
		, onAcceptCallbackQueued(false)
	{

	}

	SocketTCP::~SocketTCP()
	{
		disconnect(net::DisconnectType::socketDestructor);
		stopAccepting();
		waitUntilFinishedReceiving();
		waitUntilFinishedAccepting();
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (acceptor)
		{
			delete acceptor;
			acceptor = nullptr;
		}
		SPEHS_ASSERT(spehsAcceptThread == nullptr);

		clearReceivedPackets();
	}
	
	void SocketTCP::waitUntilFinishedReceiving()
	{
		bool wait = true;
		while (wait)
		{
			wait = isReceiving();
		}
	}

	void SocketTCP::waitUntilFinishedAccepting()
	{
		bool wait = true;
		while (wait)
		{
			wait = isAccepting();
		}

		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (spehsAcceptThread)
		{
			spehsAcceptThread->join();
			delete spehsAcceptThread;
			spehsAcceptThread = nullptr;
		}
	}

	void SocketTCP::waitUntilReceivedHandshake(const se::time::Time timeout)
	{
		bool wait;
		const se::time::Time beginTime = se::time::now();
		do
		{
			if (se::time::now() - beginTime > timeout)
				return;

			std::lock_guard<std::recursive_mutex> lock(mutex);
			wait = !handshakeReceived;
		} while (wait);
	}

	void SocketTCP::clearReceivedPackets()
	{
		std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
		for (size_t i = 0; i < receivedPackets.size(); i++)
			delete receivedPackets[i];
		receivedPackets.clear();
	}

	bool SocketTCP::connect(const net::Endpoint& endpoint)
	{
		RAIIMutexVariableSetter<bool, std::recursive_mutex> connectingSetter(connecting, true, mutex);

		//Stop receiving (outside the main mutex!)
		stopReceiving();

		{
			std::lock_guard<std::recursive_mutex> lock(mutex);

			if (connected)
			{
				se::log::info("SocketTCP::connect: failed to connect. Socket is already at the connected state!");
				return false;
			}

			//Start a new connection
			handshakeSent = false;
			handshakeReceived = false;

			//Resolve the remote endpoint
			boost::system::error_code error;
			boost::asio::ip::tcp::resolver resolverTCP(ioService.getImplementationRef());
			boost::asio::ip::tcp::resolver::query query(endpoint.address.toString(), endpoint.port.toString());
			boost::asio::ip::tcp::endpoint asioEndpoint = *resolverTCP.resolve(query, error);
			if (error)
			{
				se::log::info("SocketTCP::connect: failed to resolve the endpoint. Boost asio error: " + error.message());
				return false;
			}
			else
			{
				if (debugLogLevel >= 1)
					se::log::info("SocketTCP::connect: successfully resolved the remote endpoint.");
			}

			//Connect to the remote boost socket
			try
			{
				socket.connect(asioEndpoint, error);
			}
			catch (std::exception& exception)
			{
				se::log::info(std::string("SocketTCP::connect: failed to connect. Exception thrown: ") + exception.what());
				return false;
			}
			if (error)
			{
				se::log::info("SocketTCP::connect: failed to connect. Boost asio error: " + error.message());
				return false;
			}
			else
			{
				if (debugLogLevel >= 1)
					se::log::info("SocketTCP::connect: successfully connected the remote boost::asio::ip::tcp::socket.");
			}

			//Expect an incoming handshake after sending one
			startReceiving(onReceiveCallback);

			//Send the spehs handshake
			WriteBuffer buffer;
			const net::Handshake handshake;
			buffer.write(handshake);
			handshakeSent = sendPacket(buffer, net::PacketType::handshake);
			if (handshakeSent)
			{
				if (debugLogLevel >= 1)
					se::log::info("SocketTCP::connect: successfully sent handshake to the remote endpoint.");
			}
			else
			{
				se::log::info("SocketTCP::connect: failed to connect. Failed to send handshake.");
				return false;//If sending the handshake fails, connection was not successful
			}
		}

		//Wait until received handshake
		waitUntilReceivedHandshake(handshakeReceiveTimeout);
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (!handshakeReceived)
			{
				se::log::info("SocketTCP::connect: failed to connect! No response handshake received!");
				return false;
			}
		}
		if (debugLogLevel >= 1)
			se::log::info("SocketTCP::connect: successfully received handshake from the remote endpoint. Socket is now in connected state.");
		
		//All done, socket is now at connected state!
		std::lock_guard<std::recursive_mutex> lock(mutex);
		connected = true;
		lastReceiveTime = se::time::now();

		return true;
	}

	void SocketTCP::disconnect()
	{
		disconnect(net::DisconnectType::unspecified);
	}

	void SocketTCP::disconnect(const net::DisconnectType disconnectType)
	{
		//Stop receiving (outside the main mutex!)
		stopReceiving();

		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (!connected)
			return;

		if (disconnectType != net::DisconnectType::doNotSendDisconnectPacket)
		{//Try sending the disconnect packet before disconnecting
			WriteBuffer buffer;
			buffer.write(disconnectType);
			sendPacket(buffer, net::PacketType::disconnect);
		}
		
		//Reset the connection state
		connected = false;
		handshakeSent = false;
		handshakeReceived = false;
	}

	void SocketTCP::stopReceiving()
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
					se::log::info(e.what());
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
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (acceptor)
		{
			try
			{
				acceptor->close();
			}
			catch (std::exception& e)
			{
				se::log::info(e.what());
			}
		}
	}

	bool SocketTCP::sendPacket(const WriteBuffer& buffer, const net::PacketType packetType)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		boost::system::error_code error;

		if (!connected && !connecting && !accepting)
		{//Can only send user defined packets in the connected state
			se::log::info("SocketTCP: cannot send a packet. Socket is neither connected, connecting nor accepting.");
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
				se::log::warning("SocketTCP: failed to send packet's spehs header! Boost asio error: " + error.message());
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
				se::log::warning("SocketTCP: failed to send packet! Boost asio error: " + error.message());
				return false;
			}
		}

		if (debugLogLevel >= 2)
			se::log::info("SocketTCP: packet sent. Contents: 4(packet byte size) + 1(packet type) + " + std::to_string(buffer.getOffset()) + "(data)");
		return true;
	}

	bool SocketTCP::resizeReceiveBuffer(const size_t newSize)
	{
		if (newSize < sizeof(ExpectedBytesType))
			return false;
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (receiving)
			return false;
		receiveBuffer.resize(newSize);
		return true;
	}

	bool SocketTCP::startReceiving(const std::function<bool(ReadBuffer&)> callbackFunction)
	{
		if (isReceiving())
		{
			se::log::info("SocketTCP failed to start receiving. Socket is already receiving.");
			return false;
		}

		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (!connected && !connecting && !accepting)
		{
			se::log::warning("SocketTCP: failed to start receiving. Socket is neither connected, connecting nor accepting.");
			return false;
		}
		if (!socket.is_open())
		{//This should never happen as the connected status should guarantee socket being open...
			se::log::info("SocketTCP failed to start receiving. Socket has not been opened.");
			return false;
		}

		receiving = true;
		lastReceiveTime = se::time::now();
		onReceiveCallback = callbackFunction;
		clearReceivedPackets();
		resumeReceiving();

		if (debugLogLevel >= 2)
		{
			if (expectedBytes > 0)
				se::log::info("SocketTCP successfully started receiving. Expecting bytes: " + std::to_string(expectedBytes) + ".");
			else
				se::log::info("SocketTCP successfully started receiving. Expecting bytes header.");
		}

		return true;
	}

	void SocketTCP::resumeReceiving()
	{
		if (expectedBytes == 0)
		{//Receive header
			SPEHS_ASSERT(receiveBuffer.size() >= sizeof(expectedBytes));
			boost::asio::async_read(socket, boost::asio::buffer(&receiveBuffer[0], sizeof(expectedBytes)),
				boost::bind(&SocketTCP::receiveHandler,
					this, boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
		else
		{//Receive data
			if (receiveBuffer.size() < expectedBytes)
			{
				if (debugLogLevel >= 1)
					se::log::info("SocketTCP: the current receive buffer is insufficient! Expanding to " + std::to_string(expectedBytes));
				receiveBuffer.resize(expectedBytes);
			}
			boost::asio::async_read(socket, boost::asio::buffer(&receiveBuffer[0], expectedBytes),
				boost::bind(&SocketTCP::receiveHandler,
					this, boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
	}

	void SocketTCP::update()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);

		//onAccept callback
		if (onAcceptCallbackQueued)
		{
			onAcceptCallbackQueued = false;
			onAcceptCallback(*this);
		}

		//Received packets
		std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
		if (onReceiveCallback)
		{
			for (size_t i = 0; i < receivedPackets.size(); i++)
			{
				ReadBuffer buffer(receivedPackets[i]->data(), receivedPackets[i]->size());
				onReceiveCallback(buffer);
			}
		}
		clearReceivedPackets();
	}

	void SocketTCP::receiveHandler(const boost::system::error_code& error, std::size_t bytes)
	{
		if (debugLogLevel >= 2)
			se::log::info("SocketTCP receive handler received " + std::to_string(bytes) + " bytes.");

		std::lock_guard<std::recursive_mutex> lock(mutex);
		receiving = false;
		lastReceiveTime = se::time::now();

		if (debugLogLevel >= 3)
		{
			se::log::info("Received bytes: ");
			for (size_t i = 0; i < bytes; i++)
				se::log::info("    [" + std::to_string(i) + "] " + se::toHexString(receiveBuffer[i]));
		}

		if (error)
		{
			if (error == boost::asio::error::eof)
			{//Connection gracefully closed
				se::log::info("SocketTCP disconnected. Remote socket closed connection.");
				disconnect(net::DisconnectType::doNotSendDisconnectPacket);
				return;
			}
			else if (error == boost::asio::error::connection_reset)
			{//Disconnect
				se::log::info("SocketTCP disconnected. Remote socket closed connection.");
				disconnect(net::DisconnectType::doNotSendDisconnectPacket);
				return;
			}
			else if (error == boost::asio::error::connection_aborted ||
				error == boost::asio::error::connection_refused ||
				error == boost::asio::error::bad_descriptor ||
				error == boost::asio::error::operation_aborted)
			{
				se::log::info("Boost asio error: " + std::to_string(error.value()));
				if (error == boost::asio::error::connection_aborted)
					se::log::info("Closing client: boost asio error: connection_aborted");
				if (error == boost::asio::error::connection_refused)
					se::log::info("Closing client: boost asio error: connection_refused");
				if (error == boost::asio::error::bad_descriptor)
					se::log::info("Closing client: boost asio error: bad_descriptor");
				if (error == boost::asio::error::operation_aborted)
					se::log::info("Closing client: boost asio error: operation_aborted");
				disconnect(net::DisconnectType::doNotSendDisconnectPacket);
				return;
			}
			else
			{//Ignored error
				se::log::info("Ignored ASIO error: " + std::to_string(error.value()) + ": " + error.message());
			}
		}

		if (bytes)
		{
			ReadBuffer readBuffer(&receiveBuffer[0], bytes);
			if (expectedBytes == 0)
			{//Header received
				readBuffer.read(expectedBytes);
				resumeReceiving();
			}
			else if (expectedBytes == bytes)
			{//Data received

				//Read buffer
				const bool keepReceiving = spehsReceiveHandler(readBuffer);
				expectedBytes = 0;//Begin to expect header next
				if (keepReceiving)
					resumeReceiving();
			}
			else
			{
				se::log::warning("SocketTCP received unexpected amount of bytes! Canceling receiving!");
			}
		}
	}

	bool SocketTCP::spehsReceiveHandler(ReadBuffer& buffer)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);

		//Read packet type
		net::PacketType packetType;
		buffer.read(packetType);
		if (debugLogLevel >= 2)
			se::log::info("SocketTCP protocol receive handler received packet of type: " + std::to_string((int)packetType) + ", " + std::to_string(buffer.getBytesRemaining()) + " bytes.");

		//Process packet
		switch (packetType)
		{
		case net::PacketType::undefined:
		{
			const size_t userBytes = buffer.getBytesRemaining();
			if (debugLogLevel >= 2)
				se::log::info("SocketTCP received user defined packet. Bytes: " + std::to_string(userBytes));

			if (onReceiveCallback)
			{
				//Push to received packets queue
				std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
				receivedPackets.push_back(new std::vector<uint8_t>(userBytes));
				memcpy(receivedPackets.back()->data(), buffer[buffer.getOffset()], userBytes);
				return true;
			}
			else
			{
				se::log::warning("SocketTCP::protocolReceiveHandler: no receive handler provided for the user defined packet.");
				return false;
			}
		}
		case net::PacketType::disconnect:
		{
			net::DisconnectType disconnectType;
			buffer.read(disconnectType);
			disconnect(net::DisconnectType::doNotSendDisconnectPacket);
			se::log::info("SocketTCP: remote socket gracefully disconnected. Disconnect type was: " + std::to_string((uint8_t)disconnectType));
			return false;
		}
		case net::PacketType::handshake:
		{
			if (handshakeReceived)
			{
				se::log::warning("A handshake for the current connection has already been received!");
				return false;
			}
			net::Handshake handshake;
			buffer.read(handshake);

			if (handshake.isValid())
			{//VALID HANDSHAKE
				handshakeReceived = true;
				if (debugLogLevel >= 1)
					se::log::info("SocketTCP valid handshake received.");
			}
			else
			{//INVALID HANDSHAKE -> DISCARD
				handshakeReceived = true;
				se::log::warning("Received an invalid spehs handshake!");
			}
			return false;
		}
		}
		return false;
	}

	bool SocketTCP::startAccepting(const net::Port& port, const std::function<void(SocketTCP&)> callbackFunction)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (accepting)
		{
			se::log::info("SocketTCP failed to start accepting! Socket is already accepting!");
			return false;
		}
		if (connected)
		{
			se::log::warning("SocketTCP failed start accepting! Socket is currently connected!");
			return false;
		}
		SPEHS_ASSERT(!handshakeSent);
		SPEHS_ASSERT(!handshakeReceived);
		onAcceptCallbackQueued = false;

		if (acceptor == nullptr)
			acceptor = new boost::asio::ip::tcp::acceptor(ioService.getImplementationRef());
		boost::system::error_code error;
		const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);

		//Open acceptor
		acceptor->open(endpoint.protocol(), error);
		if (error)
		{
			se::log::warning("Failed to open acceptor! Boost asio error: " + error.message());
			return false;
		}

		//Bind acceptor
		acceptor->bind(endpoint, error);
		if (error)
		{
			se::log::warning("Failed to bind acceptor! Boost asio error: " + error.message());
			return false;
		}
		if (!acceptor->is_open())
		{
			se::log::warning("Boost acceptor failed to open!");
			return false;
		}

		//Make acceptor listen
		acceptor->listen(boost::asio::socket_base::max_connections, error);
		if (error)
		{
			se::log::warning("Failed to make acceptor listen! Boost asio error: " + error.message());
			return false;
		}

		//Set the callback function
		onAcceptCallback = callbackFunction;

		//Start accepting asynchoronously
		accepting = true;
		acceptor->async_accept(socket, boost::bind(&SocketTCP::onAccept, this, boost::asio::placeholders::error));

		return true;
	}

	void SocketTCP::onAccept(const boost::system::error_code error)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		SPEHS_ASSERT(!isConnected());
		SPEHS_ASSERT(!isReceiving());
		SPEHS_ASSERT(isAccepting());
		SPEHS_ASSERT(acceptor);
		SPEHS_ASSERT(!spehsAcceptThread);
		acceptor->close();
		if (error)
		{
			se::log::warning("SocketTCP failed to accept an incoming connection! Boost asio error: " + error.message() + "Accepting has stopped.");
			disconnect(net::DisconnectType::doNotSendDisconnectPacket);
			accepting = false;
			return;
		}
		else
		{
			spehsAcceptThread = new std::thread(&SocketTCP::spehsAccept, this);
		}
	}

	void SocketTCP::spehsAccept()
	{
		//Wait until received remote handshake
		if (debugLogLevel >= 1)
			se::log::info("Accepting SocketTCP expecting a handshake...");

		//Start expecting an incoming handshake (connector sends first)
		startReceiving(onReceiveCallback);
		waitUntilReceivedHandshake(handshakeReceiveTimeout);

		{//Check if received the handshake in time
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (!handshakeReceived)
			{
				se::log::info("SocketTCP failed to accept an incoming connection! No response handshake received!");
				disconnect(net::DisconnectType::doNotSendDisconnectPacket);
				onAcceptCallbackQueued = true;
				return;
			}
		}
		if (debugLogLevel >= 1)
			se::log::info("Accepting SocketTCP received a handshake.");
		
		//Send a response handshake
		WriteBuffer buffer;
		const net::Handshake handshake;
		buffer.write(handshake);
		if (sendPacket(buffer, net::PacketType::handshake))
		{
			handshakeSent = true;
			if (debugLogLevel >= 1)
				se::log::info("Accepting SocketTCP sent a handshake.");
		}
		else
		{
			se::log::info("SocketTCP failed to accept an incoming connection! Could not send handshake!");
			disconnect(net::DisconnectType::doNotSendDisconnectPacket);

			std::lock_guard<std::recursive_mutex> lock(mutex);
			onAcceptCallbackQueued = true;
			accepting = false;
			return;
		}

		//Socket is now in the connected state!
		std::lock_guard<std::recursive_mutex> lock(mutex);
		accepting = false;
		connected = true;
		lastReceiveTime = se::time::now();
		onAcceptCallbackQueued = true;
	}

	net::Address SocketTCP::getRemoteAddress() const
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (!isConnected())
		{
			se::log::info("SocketTCP: cannot retrieve remote endpoint address! Socket is not connected!");
			return "0.0.0.0";
		}
		boost::asio::ip::tcp::endpoint endpoint = socket.remote_endpoint();
		boost::asio::ip::address address = endpoint.address();
		boost::asio::ip::address_v4 address_v4 = address.to_v4();
		return address_v4.to_string();
	}

	net::Port SocketTCP::getRemotePort() const
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (!isConnected())
		{
			se::log::info("SocketTCP: cannot retrieve remote endpoint port! Socket is not connected!");
			return 0;
		}
		boost::asio::ip::tcp::endpoint endpoint = socket.remote_endpoint();
		return endpoint.port();
	}

	net::Endpoint SocketTCP::getRemoteEndpoint() const
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (!isConnected())
		{
			se::log::info("SocketTCP: cannot retrieve remote endpoint port! Socket is not connected!");
			return net::Endpoint::invalid;
		}
		return net::Endpoint(socket.remote_endpoint().address().to_v4().to_string(), socket.remote_endpoint().port());
	}
	
	bool SocketTCP::isAccepting() const
	{
		std::lock_guard<std::recursive_mutex> locks(mutex);
		return accepting;
	}
	
	bool SocketTCP::isReceiving() const
	{
		std::lock_guard<std::recursive_mutex> locks(mutex);
		return receiving;
	}

	bool SocketTCP::isConnected() const
	{
		std::lock_guard<std::recursive_mutex> locks(mutex);
		if (se::time::now() - lastReceiveTime >= connectionTimeout)
			return false;
		return connected;
	}
//		std::string ghostName;
//		buffer.read(ghostName);
//		const std::string ghostPath = workingDirectory + "/" + ghostName + ".exe";
//		if (fileExists(ghostPath))
//		{//Ghost file exists
//			//Search for the specified ghost program on local drive. If ghost program exists, launch it and pass it the remote endpoint to connect to.
//
//#ifdef _WIN32
//			// additional information
//			STARTUPINFO si;
//			PROCESS_INFORMATION pi;
//			// set the size of the structures
//			ZeroMemory(&si, sizeof(si));
//			si.cb = sizeof(si);
//			ZeroMemory(&pi, sizeof(pi));
//			// start the program up
//			CreateProcess(ghostPath.c_str(), // format: "E:/Ohjelmointi/Projects/Aria/Solution/bin/Ghost0.exe"
//				"param0",		// Command line
//				NULL,           // Process handle not inheritable
//				NULL,           // Thread handle not inheritable
//				FALSE,          // Set handle inheritance to FALSE
//				CREATE_NEW_CONSOLE,//Creation flags
//				NULL,           // Use parent's environment block
//				NULL,           // Use parent's starting directory 
//				&si,            // Pointer to STARTUPINFO structure
//				&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
//			);
//			// Close process and thread handles.
//			CloseHandle(pi.hProcess);
//			CloseHandle(pi.hThread);
//#elif
//#warning Missing implementation for ghost launching on current platform
//#endif
}