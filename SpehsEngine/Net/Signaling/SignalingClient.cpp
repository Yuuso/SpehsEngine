#include "stdafx.h"
#include "SpehsEngine/Net/Signaling/SignalingClient.h"

#include "SpehsEngine/Net/Connection.h"
#include "SpehsEngine/Net/SocketTCP.h"
#include "SpehsEngine/Net/Signaling/SignalingPackets.h"
#include "steam/steamnetworkingcustomsignaling.h"
#include "steam/isteamnetworkingutils.h"
#include "steam/steamnetworkingsockets_flat.h"
#pragma optimize("", off) // nocommit

/*
namespace se
{
	namespace net
	{
		SignalingClient* signalingClientSingleton = nullptr; // HACK
		ISteamNetworkingConnectionSignaling* createSignalingForConnection(const SteamNetworkingIdentity& steamNetworkingIdentity, SteamNetworkingErrMsg& error);

		struct SignalingReceivedContext : public ISteamNetworkingSignalingRecvContext
		{
			ISteamNetworkingConnectionSignaling* OnConnectRequest(HSteamNetConnection hConn, const SteamNetworkingIdentity& identityPeer, int nLocalVirtualPort) final
			{
				SteamNetworkingErrMsg ignoreErrMsg;
				return createSignalingForConnection(identityPeer, ignoreErrMsg);
			}
			void SendRejectionSignal(const SteamNetworkingIdentity& identityPeer, const void* pMsg, int cbMsg) final
			{
				// We'll just silently ignore all failures. This is actually the more secure way to handle it in many cases.
			}
		};

		struct SignalingClient::Impl
		{
			Impl(IOService &ioService, const Endpoint& serverEndpoint)
				: socket(ioService)
			{
				if (socket.connect(serverEndpoint))
				{
					SteamNetworkingIdentity selfSteamNetworkingIdentity;
					selfSteamNetworkingIdentity.Clear();
					SteamNetworkingSockets()->GetIdentity(&selfSteamNetworkingIdentity);
					assert(!identitySelf.IsInvalid());
					assert(!identitySelf.IsLocalHost());
					SteamNetworkingIdentityRender selfSteamNetworkingIdentityRender(selfSteamNetworkingIdentity);
					SignalingEntryPacket packet;
					packet.identity = selfSteamNetworkingIdentityRender.c_str();
					WriteBuffer writeBuffer;
					writeBuffer.write(packet);
					socket.sendPacket(writeBuffer);
					socket.setOnReceiveCallback(
						[](ReadBuffer& readBuffer)
						{
							SignalingReceivedContext signalingReceivedContext;
							SteamNetworkingSockets()->ReceivedP2PCustomSignal(readBuffer[0], int(readBuffer.getSize()), &signalingReceivedContext);
						});
				}
				else
				{
					se::log::warning("Failed to connect to remote SignalingServer");
				}
			}
			void update()
			{
				socket.update();
			}
			SocketTCP socket;
		};

		struct ConnectionSignaling : public ISteamNetworkingConnectionSignaling
		{
			ConnectionSignaling(const std::string_view _peerIdentity)
				: peerIdentity(_peerIdentity)
			{
			}

			/// Called to send a rendezvous message to the remote peer.  This may be called
			/// from any thread, at any time, so you need to be thread-safe!  Don't take
			/// any locks that might hold while calling into SteamNetworkingSockets functions,
			/// because this could lead to deadlocks.
			///
			/// Note that when initiating a connection, we may not know the identity
			/// of the peer, if you did not specify it in ConnectP2PCustomSignaling.
			///
			/// Return true if a best-effort attempt was made to deliver the message.
			/// If you return false, it is assumed that the situation is fatal;
			/// the connection will be closed, and Release() will be called
			/// eventually.
			///
			/// Signaling objects will not be shared between connections.
			/// You can assume that the same value of hConn will be used
			/// every time.
			/// ***************************************************************************************************
			// This is called from SteamNetworkingSockets to send a signal.  This could be called from any thread,
			// so we need to be threadsafe, and avoid doing slow stuff or calling back into SteamNetworkingSockets
			bool SendSignal(HSteamNetConnection hConn, const SteamNetConnectionInfo_t& info, const void* messageData, int messageSize) final
			{
				//SignalingEntryPacket packet;
				//packet.identity = peerIdentity;
				//std::string signal;
				//signal.reserve(peerIdentity.length() + messageSize * 2 + 4);
				//signal.append(peerIdentity);
				//signal.push_back(' ');
				//for (const uint8_t* p = (const uint8_t*)messageData; messageSize > 0; --messageSize, ++p)
				//{
				//	static const char hexdigit[] = "0123456789abcdef";
				//	signal.push_back(hexdigit[*p >> 4U]);
				//	signal.push_back(hexdigit[*p & 0xf]);
				//}
				//signal.push_back('\n');

				se_assert(signalingClientSingleton);
				if (signalingClientSingleton)
				{
					SignalingDataPacket packet;
					packet.identity = peerIdentity;
					packet.data.resize(size_t(messageSize));
					memcpy(packet.data.data(), messageData, messageSize);
					WriteBuffer writeBuffer;
					writeBuffer.write(packet);
					return signalingClientSingleton->impl->socket.sendPacket(writeBuffer);
				}
				else
				{
					return false;
				}
			}
			void Release() final
			{
				delete this;
			}
			const std::string peerIdentity;
		};
		ISteamNetworkingConnectionSignaling* createSignalingForConnection(const SteamNetworkingIdentity& steamNetworkingIdentity, SteamNetworkingErrMsg& error)
		{
			if (signalingClientSingleton)
			{
				const SteamNetworkingIdentityRender steamNetworkingIdentityRender(steamNetworkingIdentity);
				return new ConnectionSignaling(steamNetworkingIdentityRender.c_str());
			}
			else
			{
				(void)error;
				return nullptr;
			}
		}

		SignalingClient::SignalingClient(IOService& ioService, const Endpoint& serverEndpoint)
			: impl(new Impl(ioService, serverEndpoint))
		{
			se_assert(!signalingClientSingleton && "Multiple SignalingClients not supported");
			if (!signalingClientSingleton)
			{
				signalingClientSingleton = this;
				if (!SteamNetworkingUtils()->SetGlobalConfigValuePtr(k_ESteamNetworkingConfig_Callback_CreateConnectionSignaling, (void*)createSignalingForConnection))
				{
					se::log::error("Failed to set k_ESteamNetworkingConfig_Callback_CreateConnectionSignaling");
				}
			}
		}
		SignalingClient::~SignalingClient()
		{
			if (signalingClientSingleton == this)
			{
				signalingClientSingleton = nullptr;
			}
		}
		void SignalingClient::update()
		{
			impl->update();
		}
	}
}
*/
