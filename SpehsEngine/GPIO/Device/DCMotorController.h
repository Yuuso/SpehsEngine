#pragma once
#include <thread>
#include <mutex>
#include "SpehsEngine/GPIO/Pin.h"
#include "SpehsEngine/GPIO/Device/ThreadedDevice.h"


namespace se
{
	namespace device
	{
		class DCMotorController : public ThreadedDevice
		{
		public:
			DCMotorController();
			~DCMotorController();

			//Initialization
			/* Sets the pins that are used to control the DCMotorController. */
			void setPins(const gpio::Pin pulseWidthPin, const gpio::Pin inputPin1, gpio::Pin inputPin2);

			//Control
			/* Set strength within the range of [-1.0f, 1.0f] */
			void setStrength(const float strength);
			float getStrength() const;

		private:
			void onStart() override;
			void update() override;
			void onStop() override;

			mutable std::recursive_mutex mutex;
			gpio::Pin pulseWidthPin;
			gpio::Pin inputPin1;
			gpio::Pin inputPin2;
			se::time::Time pulseWidth;
			float strength;
			const se::time::Time pulseInterval;
		};
	}
}