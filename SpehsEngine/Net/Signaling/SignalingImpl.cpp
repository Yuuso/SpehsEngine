#include "stdafx.h"
#include "SpehsEngine/Net/Signaling/SignalingImpl.h"

#include "SpehsEngine/Net/Signaling/SignalingPackets.h"
#include "steam/isteamnetworkingsockets.h"


namespace se
{
	namespace net
	{
		std::recursive_mutex OutConnectionSignaling::staticMutex;
		std::unique_ptr<IOService> OutConnectionSignaling::staticIoService;
		std::unordered_map<Endpoint, std::unique_ptr<SocketTCP>> OutConnectionSignaling::staticSockets;

		std::recursive_mutex AcceptorP2P::staticMutex;
		std::unique_ptr<IOService> AcceptorP2P::staticIoService;
		std::unordered_map<Endpoint, std::unique_ptr<AcceptorP2P>> AcceptorP2P::staticContainer;

		OutSignalingReceivedContext::OutSignalingReceivedContext(const se::net::Endpoint& _signalingServerEndpoint)
			: signalingServerEndpoint(_signalingServerEndpoint)
		{
		}
		void OutSignalingReceivedContext::SendRejectionSignal(const SteamNetworkingIdentity& peerSteamNetworkingIdentity, const void* pMsg, int cbMsg)
		{
			// We'll just silently ignore all failures. This is actually the more secure way to handle it in many cases.
			SteamNetworkingIdentityRender peerSteamNetworkingIdentityRender(peerSteamNetworkingIdentity);
			log::info("OutSignalingReceivedContext: silently rejecting: " + std::string(peerSteamNetworkingIdentityRender.c_str()));
		}
		ISteamNetworkingConnectionSignaling* OutSignalingReceivedContext::OnConnectRequest(HSteamNetConnection hConn, const SteamNetworkingIdentity& peerSteamNetworkingIdentity, int nLocalVirtualPort)
		{
			const NetIdentity peerNetIdentity = fromSteamNetworkingIdentity(peerSteamNetworkingIdentity); // TODO: this probably doesn't work. Edit: actually maybe it does.
			se_assert(peerNetIdentity);
			OutConnectionSignaling* const outConnectionSignaling = new OutConnectionSignaling(peerNetIdentity, signalingServerEndpoint);
			return outConnectionSignaling;
		}

		InSignalingReceivedContext::InSignalingReceivedContext(SocketTCP& _signalingServerConnectedSocket)
			: socket(_signalingServerConnectedSocket)
		{
			se_assert(socket.isConnected());
		}
		void InSignalingReceivedContext::SendRejectionSignal(const SteamNetworkingIdentity& peerSteamNetworkingIdentity, const void* pMsg, int cbMsg)
		{
			// We'll just silently ignore all failures. This is actually the more secure way to handle it in many cases.
			SteamNetworkingIdentityRender peerSteamNetworkingIdentityRender(peerSteamNetworkingIdentity);
			log::info("InSignalingReceivedContext: silently rejecting: " + std::string(peerSteamNetworkingIdentityRender.c_str()));
		}
		ISteamNetworkingConnectionSignaling* InSignalingReceivedContext::OnConnectRequest(HSteamNetConnection hConn, const SteamNetworkingIdentity& peerSteamNetworkingIdentity, int nLocalVirtualPort)
		{
			const NetIdentity peerNetIdentity = fromSteamNetworkingIdentity(peerSteamNetworkingIdentity); // TODO: this probably doesn't work. Edit: actually maybe it does.
			se_assert(peerNetIdentity);
			InConnectionSignaling* const inConnectionSignaling = new InConnectionSignaling(peerNetIdentity, socket);
			return inConnectionSignaling;
		}

		SocketTCP& OutConnectionSignaling::getStaticSocket(const Endpoint& _signalingServerEndpoint)
		{
			std::lock_guard<std::recursive_mutex> lock(staticMutex);
			se_assert(!staticIoService);
			if (!staticIoService)
			{
				staticIoService.reset(new IOService());
			}
			std::unique_ptr<SocketTCP>& socket = staticSockets[_signalingServerEndpoint];
			se_assert(!socket);
			if (!socket)
			{
				socket.reset(new SocketTCP(*staticIoService));
			}
			return *socket;
		}

		// Creates a static socket for this signaling server endpoint
		OutConnectionSignaling::OutConnectionSignaling(const NetIdentity _peerNetIdentity, const Endpoint& _signalingServerEndpoint)
			: peerNetIdentity(_peerNetIdentity)
			, staticSocketEndpoint(_signalingServerEndpoint)
			, socket(getStaticSocket(_signalingServerEndpoint))
		{
			if (socket.connect(_signalingServerEndpoint))
			{
				SteamNetworkingIdentity selfSteamNetworkingIdentity;
				selfSteamNetworkingIdentity.Clear();
				if (!SteamNetworkingSockets()->GetIdentity(&selfSteamNetworkingIdentity))
				{
					log::warning("SteamNetworkingSockets()->GetIdentity() failed");
				}
				se_assert(!selfSteamNetworkingIdentity.IsInvalid());
				se_assert(!selfSteamNetworkingIdentity.IsLocalHost());
				SignalingEntryPacket packet;
				packet.netIdentity = fromSteamNetworkingIdentity(selfSteamNetworkingIdentity);
				WriteBuffer writeBuffer;
				writeBuffer.write(packet);
				socket.sendPacket(writeBuffer);
				socket.setOnReceiveCallback(
					[_signalingServerEndpoint](ReadBuffer& readBuffer)
					{
						SignalingDataPacket packet;
						if (!packet.read(readBuffer))
						{
							se::log::warning("OutConnectionSignaling: failed to read SignalingDataPacket");
							return;
						}
						OutSignalingReceivedContext outSignalingReceivedContext(_signalingServerEndpoint);
						if (!SteamNetworkingSockets()->ReceivedP2PCustomSignal((const void*)packet.data.data(), (int)packet.data.size(), &outSignalingReceivedContext))
						{
							se::log::warning("ReceivedP2PCustomSignal failed for OutSignalingReceivedContext");
						}
					});
			}
			else
			{
				se::log::error("Failed to connect to the signaling server at: " + _signalingServerEndpoint.toString());
			}
		}
		OutConnectionSignaling::~OutConnectionSignaling()
		{
			if (staticSocketEndpoint)
			{
				std::lock_guard<std::recursive_mutex> lock(staticMutex);
				staticSockets.erase(*staticSocketEndpoint);
				staticIoService.reset();
			}
			else
			{
				socket.setOnReceiveCallback();
			}
		}
		bool OutConnectionSignaling::SendSignal(HSteamNetConnection hConn, const SteamNetConnectionInfo_t& info, const void* messageData, int messageSize)
		{
			SignalingDataPacket packet;
			packet.netIdentity = peerNetIdentity;
			packet.data.resize(size_t(messageSize));
			memcpy(packet.data.data(), messageData, messageSize);
			WriteBuffer writeBuffer;
			writeBuffer.write(packet);
			return socket.sendPacket(writeBuffer);
		}
		void OutConnectionSignaling::Release()
		{
			delete this;
		}

		InConnectionSignaling::InConnectionSignaling(const NetIdentity _peerNetIdentity, SocketTCP& _socket)
			: peerNetIdentity(_peerNetIdentity)
			, socket(_socket)
		{
			se_assert(socket.isConnected());
		}
		bool InConnectionSignaling::SendSignal(HSteamNetConnection hConn, const SteamNetConnectionInfo_t& info, const void* messageData, int messageSize)
		{
			SignalingDataPacket packet;
			packet.netIdentity = peerNetIdentity;
			packet.data.resize(size_t(messageSize));
			memcpy(packet.data.data(), messageData, messageSize);
			WriteBuffer writeBuffer;
			writeBuffer.write(packet);
			return socket.sendPacket(writeBuffer);
		}
		void InConnectionSignaling::Release()
		{
			delete this;
		}

		IOService& AcceptorP2P::getStaticIoService()
		{
			std::lock_guard<std::recursive_mutex> lock(staticMutex);
			if (!staticIoService)
			{
				staticIoService.reset(new IOService());
			}
			return *staticIoService;
		}
		void AcceptorP2P::eraseFromStaticContainer(const Endpoint signalingServerEndpoint)
		{
			std::lock_guard<std::recursive_mutex> lock(staticMutex);
			std::unordered_map<Endpoint, std::unique_ptr<AcceptorP2P>>::iterator it = staticContainer.find(signalingServerEndpoint);
			if (it != staticContainer.end())
			{
				staticContainer.erase(it);
				if (staticContainer.empty())
				{
					staticIoService.reset();
				}
			}
		}
		AcceptorP2P::AcceptorP2P(const Endpoint& signalingServerEndpoint)
			: socket(getStaticIoService())
		{
			if (socket.connect(signalingServerEndpoint))
			{
				SteamNetworkingIdentity selfSteamNetworkingIdentity;
				selfSteamNetworkingIdentity.Clear();
				SteamNetworkingSockets()->GetIdentity(&selfSteamNetworkingIdentity);
				se_assert(!selfSteamNetworkingIdentity.IsInvalid());
				se_assert(!selfSteamNetworkingIdentity.IsLocalHost());
				SignalingEntryPacket packet;
				packet.netIdentity = fromSteamNetworkingIdentity(selfSteamNetworkingIdentity);
				WriteBuffer writeBuffer;
				packet.write(writeBuffer);
				socket.sendPacket(writeBuffer);
			}
		}
		void AcceptorP2P::receiveHandler(ReadBuffer& readBuffer)
		{
			SignalingDataPacket packet;
			if (!packet.read(readBuffer))
			{
				se::log::info("AcceptorP2P: failed to read SignalingDataPacket");
				return;
			}
			InSignalingReceivedContext inSignalingReceivedContext(socket);
			if (!SteamNetworkingSockets()->ReceivedP2PCustomSignal((const void*)packet.data.data(), (int)packet.data.size(), &inSignalingReceivedContext))
			{
				se::log::error("ReceivedP2PCustomSignal failed for InSignalingReceivedContext in AcceptorP2P.");
			}
		}
	}
}
