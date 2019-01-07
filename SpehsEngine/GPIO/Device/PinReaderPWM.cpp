#include "stdafx.h"
#include <cstring>
#include "SpehsEngine/GPIO/Device/PinReaderPWM.h"
#include "SpehsEngine/Core/Log.h"
#include <SpehsEngine/Core/WriteBuffer.h>
#include <SpehsEngine/Core/ReadBuffer.h>

namespace se
{
	namespace device
	{
		void PWMHistory::write(WriteBuffer& buffer) const
		{
			buffer.write(size());
			for (size_t i = 0; i < size(); i++)
				buffer.write(at(i));
		}

		bool PWMHistory::read(ReadBuffer& buffer)
		{
			size_t s;
			se_read(buffer, s);
			resize(s);
			for (size_t i = 0; i < size(); i++)
			{
				se_read(buffer, at(i));
			}
			return true;
		}

		PinReaderPWMGhost::PinReaderPWMGhost()
			: active(false)
			, requestUpdate(true)
			, pin(gpio::Pin::pin_none)
			, sampleSize(100)
			, sampleRate(se::time::fromMilliseconds(1.0f))
		{

		}

		PinReaderPWMGhost::~PinReaderPWMGhost()
		{

		}

		void PinReaderPWMGhost::syncCreate(WriteBuffer& buffer)
		{

		}

		void PinReaderPWMGhost::syncCreate(ReadBuffer& buffer)
		{

		}

		void PinReaderPWMGhost::syncRemove(WriteBuffer& buffer)
		{

		}

		void PinReaderPWMGhost::syncRemove(ReadBuffer& buffer)
		{

		}

		bool PinReaderPWMGhost::syncUpdate(const se::time::Time deltaTime)
		{
			return requestUpdate;
		}

		void PinReaderPWMGhost::syncUpdate(WriteBuffer& buffer)
		{
			buffer.write(active);
			buffer.write(pin);
			buffer.write(sampleSize);
			buffer.write(sampleRate);
			requestUpdate = false;
		}

		void PinReaderPWMGhost::syncUpdate(ReadBuffer& buffer)
		{
			bool clearHistory;
			PWMHistory newHistory;
			buffer.read(clearHistory);
			if (clearHistory)
				history.clear();
			buffer.read(newHistory);
			const size_t oldHistoryOffset = history.size();
			history.resize(history.size() + newHistory.size());
			if (newHistory.size() > 0)
			{
				memcpy(&history[oldHistoryOffset], newHistory.data(), sizeof(newHistory.front()) * newHistory.size());
			}
		}

		void PinReaderPWMGhost::setPin(const gpio::Pin newPin)
		{
			if (pin != newPin)
			{
				pin = newPin;
				requestUpdate = true;
			}
		}

		void PinReaderPWMGhost::setActive(const bool isActive)
		{
			if (active != isActive)
			{
				active = isActive;
				requestUpdate = true;
			}
		}

		void PinReaderPWMGhost::setSampleSize(const size_t size)
		{
			if (size != sampleSize)
			{
				sampleSize = size;
				requestUpdate = true;
			}
		}

		void PinReaderPWMGhost::setSampleRate(const se::time::Time interval)
		{
			if (interval != sampleRate)
			{
				sampleRate = interval;
				requestUpdate = true;
			}
		}
		
		void PinReaderPWMGhost::clearHistory()
		{
			history.clear();
		}

		

		PinReaderPWMShell::PinReaderPWMShell()
			: active(false)
			, requestUpdate(false)
			, clearHistory(false)
			, pin(gpio::Pin::pin_none)
			, highStateSampleCount(0)
			, lowStateSampleCount(0)
			, sampleSize(100)
			, sampleRate(se::time::fromMilliseconds(1.0f))
			, lastReadTime(0)
		{

		}

		PinReaderPWMShell::~PinReaderPWMShell()
		{
			stop();
			while (isRunning()) {/*Blocks*/ }
			std::lock_guard<std::recursive_mutex> lock(mutex);
		}

		void PinReaderPWMShell::setActive(const bool isActive)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (active != isActive)
			{
				active = isActive;
				if (active)
				{
					start();
				}
				else
				{
					stop();
				}
			}
		}

		void PinReaderPWMShell::setPin(const gpio::Pin newPin)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (pin != newPin)
			{
				se::log::info("Setting pin reader PWM pin to: " + std::to_string(gpio::getPinEnumAsNumber(newPin)));
				pin = newPin;
				clearHistory = true;
				history.clear();
			}
		}

		void PinReaderPWMShell::setSampleSize(const size_t size)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (sampleSize != size)
			{
				se::log::info("Setting pin reader PWM sample size to: " + std::to_string(size));
				sampleSize = size;
				clearHistory = true;
				history.clear();
			}
		}

		void PinReaderPWMShell::setSampleRate(const se::time::Time interval)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (sampleRate != interval)
			{
				se::log::info("Setting pin reader PWM sample rate to: " + std::to_string(interval.asMilliseconds()) + " ms");
				sampleRate = interval;
				clearHistory = true;
				history.clear();
			}
		}

		void PinReaderPWMShell::syncCreate(WriteBuffer& buffer)
		{

		}

		void PinReaderPWMShell::syncCreate(ReadBuffer& buffer)
		{

		}

		void PinReaderPWMShell::syncRemove(WriteBuffer& buffer)
		{

		}

		void PinReaderPWMShell::syncRemove(ReadBuffer& buffer)
		{
			stop();
			while (isRunning()) {}
		}

		bool PinReaderPWMShell::syncUpdate(const se::time::Time deltaTime)
		{
			if (sizeof(size_t) + history.size() * sizeof(float/*history element*/) > 1400)
				return true;//Default MTU is around 1500, preferably keep it below that
			return false;
		}

		void PinReaderPWMShell::syncUpdate(WriteBuffer& buffer)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			buffer.write(clearHistory);
			clearHistory = false;
			const size_t historySize = history.size();
			buffer.write(historySize);
			for (size_t i = 0; i < historySize; i++)
				buffer.write(history[i]);
			history.clear();
		}

		void PinReaderPWMShell::syncUpdate(ReadBuffer& buffer)
		{
			bool _active;					buffer.read(_active);		setActive(_active);
			gpio::Pin _pin;					buffer.read(_pin);			setPin(_pin);
			size_t _sampleSize;				buffer.read(_sampleSize);	setSampleSize(_sampleSize);
			se::time::Time _sampleRate;	buffer.read(_sampleRate);	setSampleRate(_sampleRate);
		}

		void PinReaderPWMShell::getHistory(PWMHistory& deposit) const
		{
			deposit = history;
		}

		void PinReaderPWMShell::onStart()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			history.clear();
			lastReadTime = se::time::now();
		}

		void PinReaderPWMShell::update()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			const se::time::Time now = se::time::now();
			if (now - lastReadTime >= sampleRate)
			{
				lastReadTime = now;
				const gpio::PinState state = gpio::read(pin);
				if (state == gpio::PinState::low)
					lowStateSampleCount++;
				else if (state == gpio::PinState::high)
					highStateSampleCount++;
				if (lowStateSampleCount + highStateSampleCount >= sampleSize)
				{
					history.push_back((float)highStateSampleCount / float(lowStateSampleCount + highStateSampleCount));
					lowStateSampleCount = 0;
					highStateSampleCount = 0;
				}
			}
		}

		void PinReaderPWMShell::onStop()
		{
			
		}
	}
}