#pragma once
#include <string>
#ifdef _DEBUG
#define SERIALIZABLE_DEBUG_INFO
#endif
#define SPEHS_WRITE_VAR(bufferBeginPtr, offsetVar, dataVar) { memcpy(&bufferBeginPtr[offset], &dataVar, sizeof(dataVar)); offset += sizeof(dataVar); }
#define SPEHS_READ_VAR(bufferBeginPtr, offsetVar, dataVar) { memcpy(&dataVar, &bufferBeginPtr[offset], sizeof(dataVar)); offset += sizeof(dataVar); }
#define SPEHS_WRITE_ARRAY(bufferBeginPtr, offsetVar, dataPtr, arrayLength) { memcpy(&bufferBeginPtr[offset], dataPtr, sizeof(*dataPtr) * arrayLength); offset += sizeof(*dataPtr) * arrayLength; }
#define SPEHS_READ_ARRAY(bufferBeginPtr, offsetVar, dataPtr, arrayLength) { memcpy(dataPtr, &bufferBeginPtr[offset], sizeof(*dataPtr) * arrayLength); offset += sizeof(*dataPtr) * arrayLength; }

namespace spehs
{
	/**Serializable class is a class that implements the four pure virtual functions required for data transmission:
		1. newMyType: Allows allocation of new object of this type
		2. packetSize: Size required to write class into buffer at the specified moment. Packet size can be dynamic but must remain constant between subsequental calls to packetSize and write
		3. write: writes serializable data into buffer and returns number of bytes written
		4. read: reads serializable data from buffer and return number of bytes read
		*/
	class Serializable
	{
	public:

		virtual ~Serializable() {}

		virtual Serializable* clone() const = 0;///< Make a dynamic copy of the original object
		virtual size_t packetSize() const = 0;///< Returns number of bytes required for relevant data to be sent over network/ stored on disk
		virtual size_t write(unsigned char* buffer) const = 0;///< Writes relevant data to buffer. Returns number of bytes written.
		virtual size_t read(const unsigned char* buffer) = 0;///< Reads relevant data from buffer into the data class. Returns number of bytes read.

		/* Serializable class may want to implement a toString method. */
		virtual std::string toString() const { return ""; }
	};
}