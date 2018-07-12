#pragma once
#include "SpehsEngine/GPIO/Device/ThreadedDevice.h"
#include "SpehsEngine/GPIO/Pin.h"

namespace se
{
	namespace device
	{
		/*
			Ultrasonic Distance Sensor Module
		*/
		class HC_SR04 : public ThreadedDevice
		{
		public:
			HC_SR04();
			~HC_SR04() override;

			void onStart() override;
			void update() override;
			void onStop() override;

			void setPins(const gpio::Pin trigger, const gpio::Pin echo);
			void setPollInterval(const se::time::Time time);

			float getDistance() const;

		private:
			mutable std::recursive_mutex mutex;
			gpio::Pin triggerPin;
			gpio::Pin echoPin;
			float distance;
			se::time::Time pollInterval;
		};
	}
}