#include "stdafx.h"
#include "SpehsEngine/Net/Signaling/SignalingServer.h"

#include "SpehsEngine/Net/SocketTCP.h"
#include "SpehsEngine/Net/Signaling/SignalingPackets.h"


namespace se
{
	namespace net
	{
		struct SignalingServer::Impl
		{
			struct Client
			{
				std::unique_ptr<SocketTCP> socket;
				NetIdentity netIdentity;
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
							client->socket->disconnect();
							return;
						}
						if (!packet.netIdentity)
						{
							se::log::error("SignalingEntryPacket: invalid net identity");
							client->socket->disconnect();
							return;
						}

						// Check for a duplicate net identity
						for (size_t d = 0; d < clients.size(); d++)
						{
							if (clients[d]->netIdentity == packet.netIdentity)
							{
								se::log::error("SignalingEntryPacket duplicate net identity found");
								for (size_t r = 0; r < clients.size(); r++)
								{
									if (clients[r].get() == client)
									{
										clients[r]->socket->disconnect();
										std::swap(clients[r], clients.back());
										clients.pop_back();
										break;
									}
								}
								return;
							}
						}

						// Start relaying signaling data packets
						client->netIdentity = packet.netIdentity;
						client->socket->setOnReceiveCallback(
							[this, client](ReadBuffer& readBuffer)
							{
								SignalingDataPacket packet;
								if (!packet.read(readBuffer))
								{
									se::log::error("Failed to read SignalingDataPacket");
									client->socket->disconnect();
									return;
								}
								for (std::unique_ptr<Client>& client2 : clients)
								{
									if (client2->netIdentity == packet.netIdentity)
									{
										packet.netIdentity = client->netIdentity;
										WriteBuffer writeBuffer;
										writeBuffer.write(packet);
										if (!client2->socket->sendPacket(writeBuffer))
										{
											se::log::error("Failed to redirect SignalingDataPacket");
											client2->socket->disconnect();
										}
										return;
									}
								}
								se::log::info("SignalingDataPacket received for unknown net identity: " + packet.netIdentity.toString());
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
				}
				for (size_t i = 0; i < clients.size(); i++)
				{
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
