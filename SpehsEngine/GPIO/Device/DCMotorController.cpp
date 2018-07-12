#include "stdafx.h"
#include "SpehsEngine/GPIO/Device/DCMotorController.h"
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Core/Log.h"
#include <cmath>
#include <bcm2835.h>


namespace se
{
	namespace device
	{
		DCMotorController::DCMotorController()
			: pulseWidthPin(gpio::Pin::pin_none)
			, inputPin1(gpio::Pin::pin_none)
			, inputPin2(gpio::Pin::pin_none)
			, pulseWidth(0)
			, strength(0.0f)
			, pulseInterval(se::time::fromMilliseconds(2.0f))
		{

		}

		DCMotorController::~DCMotorController()
		{
			stop();
			while (isRunning()) {/*Blocks*/ }
		}

		void DCMotorController::setPins(const gpio::Pin _pulseWidthPin, const gpio::Pin _inputPin1, gpio::Pin _inputPin2)
		{
			gpio::setPinAsOutput(_pulseWidthPin);
			gpio::setPinAsOutput(_inputPin1);
			gpio::setPinAsOutput(_inputPin2);
			std::lock_guard<std::recursive_mutex> lock(mutex);
			pulseWidthPin = _pulseWidthPin;
			inputPin1 = _inputPin1;
			inputPin2 = _inputPin2;
		}

		void DCMotorController::setStrength(const float newStrength)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);

			//Cap strength in range
			strength = newStrength;
			if (strength < -1.0f)
				strength = -1.0f;
			else if (strength > 1.0f)
				strength = 1.0f;

			//Calculate pulse width
			pulseWidth = pulseInterval * abs(strength);

			//Polarity
			if (strength > 0.0f)
			{
				se::gpio::disable(inputPin1);
				se::gpio::enable(inputPin2);
			}
			else if (strength < 0.0f)
			{
				se::gpio::enable(inputPin1);
				se::gpio::disable(inputPin2);
			}
			else
			{
				se::gpio::disable(inputPin1);
				se::gpio::disable(inputPin2);
			}
		}

		float DCMotorController::getStrength() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return strength;
		}

		void DCMotorController::onStart()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			setStrength(getStrength());
		}

		void DCMotorController::update()
		{
			mutex.lock();
			const se::gpio::Pin _pulseWidthPin = pulseWidthPin;
			const se::time::Time _pulseWidth = pulseWidth;
			const se::time::Time _pulseInterval = pulseInterval;
			mutex.unlock();

			se::gpio::enable(_pulseWidthPin);
			se::time::delay(_pulseWidth);
			se::gpio::disable(_pulseWidthPin);
			se::time::delay(_pulseInterval - _pulseWidth);
		}

		void DCMotorController::onStop()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			se::gpio::disable(inputPin1);
			se::gpio::disable(inputPin2);
		}
	}
}


/*

const se::time::Time pulseInterval = se::time::fromNanoseconds(2000000);
int direction = 1;
se::time::Time data = 0;

while (keepRunning)
{
std::lock_guard<std::recursive_mutex> lock(mutex);

if (data <= 0)
direction = 1000;
else if (data >= pulseInterval)
direction = -1000;
data += direction;
se::gpio::enable(pulseWidthPin);
se::time::delay(data);
se::gpio::disable(pulseWidthPin);
se::time::delay(pulseInterval - data);

keepRunning = !stopRequested;
}

*/