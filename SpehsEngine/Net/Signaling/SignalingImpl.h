#pragma once

#include "SpehsEngine/Net/Endpoint.h"
#include "SpehsEngine/Net/NetIdentity.h"
#include "SpehsEngine/Net/SocketTCP.h"
#include "steam/isteamnetworkingutils.h"
#include "steam/steamnetworkingcustomsignaling.h"


namespace se
{
	namespace net
	{
		struct OutSignalingReceivedContext : public ISteamNetworkingSignalingRecvContext
		{
			OutSignalingReceivedContext(const Endpoint& _signalingServerEndpoint);
			void SendRejectionSignal(const SteamNetworkingIdentity& peerSteamNetworkingIdentity, const void* pMsg, int cbMsg) final;
			ISteamNetworkingConnectionSignaling* OnConnectRequest(HSteamNetConnection hConn, const SteamNetworkingIdentity& peerSteamNetworkingIdentity, int nLocalVirtualPort) final;

			const Endpoint signalingServerEndpoint;
		};

		struct InSignalingReceivedContext : public ISteamNetworkingSignalingRecvContext
		{
			InSignalingReceivedContext(ISocketTCP& _signalingServerConnectedSocket);
			void SendRejectionSignal(const SteamNetworkingIdentity& peerSteamNetworkingIdentity, const void* pMsg, int cbMsg) final;
			ISteamNetworkingConnectionSignaling* OnConnectRequest(HSteamNetConnection hConn, const SteamNetworkingIdentity& peerSteamNetworkingIdentity, int nLocalVirtualPort) final;

			ISocketTCP& socket;
		};

		// Creates a static socket for this signaling server endpoint
		struct OutConnectionSignaling : public ISteamNetworkingConnectionSignaling
		{
			static ISocketTCP* getStaticSocket(const Endpoint& _signalingServerEndpoint);

			OutConnectionSignaling(const NetIdentity _peerNetIdentity, const Endpoint& _signalingServerEndpoint);
			~OutConnectionSignaling();
			bool SendSignal(HSteamNetConnection hConn, const SteamNetConnectionInfo_t& info, const void* messageData, int messageSize) final;
			void Release() final;

			const NetIdentity peerNetIdentity;
			const Endpoint signalingServerEndpoint;
			ISocketTCP* const socket;

			static std::recursive_mutex staticMutex;
			static std::unique_ptr<IOService> staticIoService;
			static std::unordered_map<Endpoint, std::unique_ptr<ISocketTCP>> staticSockets; // NOTE: we can only have one socket connected per endpoint because of a single NetIdentity
		};

		// Re-uses a socket that is already connected to a signaling server
		struct InConnectionSignaling : public ISteamNetworkingConnectionSignaling
		{
			InConnectionSignaling(const NetIdentity _peerNetIdentity, ISocketTCP& _socket);
			bool SendSignal(HSteamNetConnection hConn, const SteamNetConnectionInfo_t& info, const void* messageData, int messageSize) final;
			void Release() final;

			const NetIdentity peerNetIdentity;
			const std::optional<Endpoint> staticSocketEndpoint; // Set when static socket is used
			ISocketTCP& socket;

			static std::recursive_mutex staticMutex;
			static std::unique_ptr<IOService> staticIoService;
			static std::unordered_map<Endpoint, std::unique_ptr<ISocketTCP>> staticSockets; // NOTE: we can only have one socket connected per endpoint
		};

		// P2P signaling accepting needs some static state between all application local connection managers because a single signaling server can only serve a single connection manager at a time
		struct AcceptorP2P
		{
			static IOService& getStaticIoService();
			static void eraseFromStaticContainer(const Endpoint signalingServerEndpoint);

			AcceptorP2P(const Endpoint& signalingServerEndpoint);
			void receiveHandler(BinaryReader& binaryReader);

			std::unique_ptr<ISocketTCP> socket;
			static std::recursive_mutex staticMutex;
			static std::unique_ptr<IOService> staticIoService;
			static std::unordered_map<Endpoint, std::unique_ptr<AcceptorP2P>> staticContainer;
		};
	}
}