#pragma once
#include <stdint.h>


namespace se
{
	namespace net
	{
		/* Specify a reason for disconnection. */
		enum class DisconnectType : uint8_t
		{
			unspecified,					//No specified reason for disconnecting
			doNotSendDisconnectPacket,		//Used from disconnect receive handler
			error,							//The program had to terminate due to an error
			socketDestructor,				//Disconnect called from the socket's destructor
			switchEndpoint,					//The endpoint is switching endpoints
			programExit,					//Used when disconnecting the socket after running the shell/ghost main program
			timeout,						//No packets received for a 
			unknownProtocol,				//The protocol seems to be uknown?
		};

		/* Spehs sockets add a packet type header into each packet. Packet type determines the contents of the packet. Currently a packet is solely limited to a single packet type. */
		enum class PacketType : uint8_t
		{
			invalid = 0,					//Not in use
			handshake,						//Packet contains spehs defined handshake data
			disconnect,						//The remote end sends a packet to indicate disconnection
			heartbeat,
			undefined = 255,				//Sent data is user defined data, which is processed by a user defined receive handler.			
		};
	}
}