#pragma once
#include <stdint.h>
#include <thread>
#include <mutex>
#include "SpehsEngine/Net/IOService.h"
#include "SpehsEngine/Net/SocketTCP.h"
#include "SpehsEngine/Net/Endpoint.h"



namespace se
{
	class ReadBuffer;
	namespace net
	{
		class SocketTCP;
		namespace aria
		{
			class Server;

			class Connector
			{
			public:
				Connector(SocketTCP& socket, const std::string& name, const std::string& counterpart, const Port& _localPortForWaiting);

				bool enter(const Endpoint& endpoint);//TODO: , const se::time::Time timeout = 0
				bool onReceive(ReadBuffer& buffer);
				void onAccept(SocketTCP& socket);

				SocketTCP& socket;
				const std::string name;
				const std::string counterpart;
				const Port localPortForWaiting;

			private:
				enum EnterResult { none, connect, accept, fail };
				std::atomic<EnterResult> enterResult;
				Endpoint connectEndpoint;

				std::mutex pingMutex;
				se::time::Time lastPingSendTime;
				se::time::Time lastPingReceiveTime;
			};

			class Client
			{
			public:
				Client(Server& _server);
				void onAccept(SocketTCP& socket);
				bool onReceive(ReadBuffer& buffer);

				Server& server;
				SocketTCP socket;
				const size_t id;

				std::mutex enterDetailsMutex;
				bool enterDetailsChanged = false;
				std::string name;
				std::string counterpart;
				Port localPortForWaiting = 0u;
			};

			/* Aria server instance, thread-safe */
			class Server
			{
				/*
				Connects socket to an aria server.
				Blocks until counterpart is found, or until timeout.
				Returns true on success, false on failure.
				*/
				friend class Client;
			public:
			public:

				Server();
				~Server();

				/* Hosts an aria server on a local port. */
				void start(const Port& localPort);

				/* Stops hosting. */
				void stop();

				bool isRunning() const;

			private:
				void run();

				mutable std::recursive_mutex mutex;

				//Run state
				std::thread* thread;
				bool keepRunning;
				bool canExitStart;
				bool threadRunning;
				Port localPort;

				//Clients
				IOService ioService;
				std::vector<Client*> clients;
			};
		}
	}
}