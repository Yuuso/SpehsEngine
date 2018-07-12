#include "SpehsEngine/GPIO/Device/HC_SR04.h"
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Core/Log.h"


namespace se
{
	namespace device
	{
		HC_SR04::HC_SR04()
			: triggerPin(gpio::pin_none)
			, echoPin(gpio::pin_none)
			, distance(0.0f)
			, pollInterval(se::time::fromMilliseconds(100))
		{
		}

		HC_SR04::~HC_SR04()
		{
			stop();
		}

		void HC_SR04::setPins(const gpio::Pin trigger, const gpio::Pin echo)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			triggerPin = trigger;
			echoPin = echo;
			gpio::setPinAsOutput(trigger);
			gpio::setPinAsInput(echo);
		}

		void HC_SR04::onStart()
		{

		}

		void HC_SR04::update()
		{
			//Lock mutec for a short while when acquiring variable values
			mutex.lock();
			const gpio::Pin trigger = triggerPin;
			const gpio::Pin echo = echoPin;
			mutex.unlock();
			
			//Fire trigger
			const se::time::Time delay1 = se::time::fromMilliseconds(2);
			const se::time::Time delay2 = se::time::fromMilliseconds(10);
			const se::time::Time beginTime = se::time::now();
			gpio::disable(trigger);
			se::time::delay(delay1);
			gpio::enable(trigger);
			se::time::delay(delay2);
			gpio::disable(trigger);

			//Measure pulse
			const se::time::Time duration = gpio::pulseIn(echo, gpio::high, se::time::fromMilliseconds(1000));

			//Update distance value
			mutex.lock();
			distance = (duration.asMicroseconds() * 0.5f) / 29.1f;
			//Delay by poll interval
			const se::time::Time interval = pollInterval + beginTime - se::time::now();
			mutex.unlock();
			se::time::delay(interval);
		}

		void HC_SR04::onStop()
		{

		}

		float HC_SR04::getDistance() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return distance;
		}
	}
}