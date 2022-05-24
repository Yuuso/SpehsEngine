#include "stdafx.h"
#include "SpehsEngine/Net/Signaling/SignalingServer.h"

#include "SpehsEngine/Net/SocketTCP.h"
#include "SpehsEngine/Net/Signaling/SignalingPackets.h"
#pragma optimize("", off) // nocommit

namespace se
{
	namespace net
	{
		struct SignalingServer::Impl
		{
			struct Client
			{
				std::unique_ptr<SocketTCP> socket;
				std::string identity;
			};

			Impl(IOService& _ioService, const Port& _port)
				: ioService(_ioService)
				, port(_port)
			{
				startAccepting();
			}

			void startAccepting()
			{
				se_assert(!acceptingSocket);
				acceptingSocket.reset(new SocketTCP(ioService));
				//acceptingSocket->open();
				//acceptingSocket->bind(port);
				acceptingSocket->startAccepting(port, std::bind(&SignalingServer::Impl::accept, this, std::placeholders::_1));
			}

			void accept(SocketTCP& socket)
			{
				// Start expecting the entry packet
				clients.push_back(std::make_unique<Client>());
				Client* const client = clients.back().get();
				client->socket.reset(acceptingSocket.release());
				client->socket->setOnReceiveCallback(
					[this, client](ReadBuffer& readBuffer)
					{
						SignalingEntryPacket packet;
						if (!packet.read(readBuffer))
						{
							se::log::error("Failed to read SignalingEntryPacket");
							return;
						}
						// TODO: validate identity
						// Start relaying signaling data packets
						client->identity = packet.identity;
						client->socket->setOnReceiveCallback(
							[this, client](ReadBuffer& readBuffer)
							{
								SignalingDataPacket packet;
								if (!packet.read(readBuffer))
								{
									se::log::error("Failed to read SignalingDataPacket");
									return;
								}
								for (std::unique_ptr<Client>& client2 : clients)
								{
									if (client2->identity == packet.identity)
									{
										packet.identity = client->identity;
										WriteBuffer writeBuffer;
										writeBuffer.write(packet);
										client2->socket->sendPacket(writeBuffer);
									}
								}
							});
					});
				startAccepting();
			}

			void update()
			{
				acceptingSocket->update();
				for (size_t i = 0; i < clients.size(); i++)
				{
					clients[i]->socket->update();
					if (!clients[i]->socket->isConnected())
					{
						std::swap(clients[i], clients.back());
						clients.pop_back();
						i--;
					}
				}
			}

			IOService& ioService;
			const Port& port;
			std::unique_ptr<SocketTCP> acceptingSocket;
			std::vector<std::unique_ptr<Client>> clients;
		};

		SignalingServer::SignalingServer(IOService& ioService, const Port& port)
			: impl(new Impl(ioService, port))
		{
		}
		
		SignalingServer::~SignalingServer()
		{
			// ~Impl()
		}

		void SignalingServer::update()
		{
			impl->update();
		}
	}
}
