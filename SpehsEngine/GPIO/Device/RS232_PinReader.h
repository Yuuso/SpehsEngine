#pragma once
#include <vector>
#include "SpehsEngine/GPIO/Device/ThreadedDevice.h"
#include "SpehsEngine/GPIO/Pin.h"

namespace spehs
{
	namespace device
	{
		class RS232_PinReader// : public spehs::device::ThreadedDevice
		{
		public:
			RS232_PinReader();
			~RS232_PinReader();

			void setPin(const gpio::Pin pin);
			void setReadInterval(const spehs::time::Time interval);
			void setTransmissionUnitLength(const uint8_t length);
			/* Can have either 1 or 2 stop bits */
			void setStopBitCount(const int count);
			void setStreamBoundaryRequiredPatternRepetitionCount(int requiredRepetitions);
			void enableParityBitCheck();
			void disableParityBitCheck();

			void retrieveReceiveBuffer(std::vector<uint8_t>& buffer);

			void onStart();
			void update();
			void onStop();
			void start()
			{
				stopRequested = false;
				onStart();
				while (!stopRequested)
					update();
				onStop();
			}
			void stop() { stopRequested = true; }
			bool stopRequested;

		private:
			bool detectStreamBoundaries();

			mutable std::recursive_mutex mutex;

			gpio::Pin pin;

			enum class ReceiveState
			{
				invalid,
				awaitingStartBit,		//Awaiting for the start bit (high state)
				receivingData,			//Receiving data bits between set intervals
				receivingParityBit,		//Receiving the parity bit
				receivingStopBits,		//Receiving the stop bit(s)
			};
			spehs::time::Time readInterval;
			spehs::time::Time nextReadTime;
			gpio::PinState previousReadState;
			int stopBitCount;
			int streamBoundaryRequiredPatternRepetitionCount;
			bool parityCheckEnabled;
			ReceiveState receiveState;
			uint8_t transmittingUnit;
			uint8_t transmissionUnitLength;
			uint8_t transmittingUnitBitIndex;
			std::vector<uint8_t> receiveBuffer;
		};
	}
}