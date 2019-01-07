#include "stdafx.h"
#include <cstring>
#include "SpehsEngine/GPIO/Device/PinReader.h"
#include "SpehsEngine/Core/Log.h"
#include <SpehsEngine/Core/WriteBuffer.h>
#include <SpehsEngine/Core/ReadBuffer.h>

namespace se
{
	namespace device
	{
		void PinReaderHistoryEntry::write(WriteBuffer& buffer) const
		{
			buffer.write(state);
			buffer.write(time);
		}

		bool PinReaderHistoryEntry::read(ReadBuffer& buffer)
		{
			se_read(buffer, state);
			se_read(buffer, time);
			return true;
		}

		void PinReaderHistory::write(WriteBuffer& buffer) const
		{
			const size_t count = size();//TODO: buffer.write(std::vector<...>)
			buffer.write(count);
			for (size_t i = 0; i < count; i++)
				buffer.write(at(i));
		}

		bool PinReaderHistory::read(ReadBuffer& buffer)
		{
			size_t count;
			se_read(buffer, count);//TODO: buffer.read(std::vector<...>)
			resize(count);
			for (size_t i = 0; i < count; i++)
			{
				se_read(buffer, at(i));
			}
			return true;
		}

		gpio::PinState PinReaderHistory::stateAtTime(const se::time::Time& time) const
		{
			if (empty() || time < front().time || time > back().time)
				return gpio::PinState::invalid;
			size_t i = 0;
			while (at(i).time < time)
			{
				i++;
			}
			if (i == 0)
				return front().state;
			if (time - at(i - 1).time < at(i).time - time)
				return at(i - 1).state;
			else
				return at(i).state;
		}



		PinReaderGhost::PinReaderGhost()
			: pin(gpio::Pin::pin_none)
			, active(false)
			, requestUpdate(true)
		{

		}

		PinReaderGhost::~PinReaderGhost()
		{

		}

		void PinReaderGhost::syncCreate(WriteBuffer& buffer)
		{

		}

		void PinReaderGhost::syncCreate(ReadBuffer& buffer)
		{

		}

		void PinReaderGhost::syncRemove(WriteBuffer& buffer)
		{

		}

		void PinReaderGhost::syncRemove(ReadBuffer& buffer)
		{

		}

		bool PinReaderGhost::syncUpdate(const se::time::Time deltaTime)
		{
			return requestUpdate;
		}

		void PinReaderGhost::syncUpdate(WriteBuffer& buffer)
		{
			buffer.write(active);
			buffer.write(pin);
			requestUpdate = false;
		}

		void PinReaderGhost::syncUpdate(ReadBuffer& buffer)
		{
			bool clearHistory;
			PinReaderHistory newHistory;
			buffer.read(clearHistory);
			if (clearHistory)
				history.clear();
			buffer.read(newHistory);
			history.insert(history.end(), newHistory.begin(), newHistory.end());
		}

		void PinReaderGhost::setPin(const gpio::Pin newPin)
		{
			if (pin != newPin)
			{
				pin = newPin;
				requestUpdate = true;
			}
		}

		void PinReaderGhost::setActive(const bool isActive)
		{
			if (active != isActive)
			{
				active = isActive;
				requestUpdate = true;
			}
		}
		
		void PinReaderGhost::clearHistory()
		{
			history.clear();
		}

		

		PinReaderShell::PinReaderShell()
			: pin(gpio::Pin::pin_none)
			, clearHistory(false)
			, active(false)
		{

		}

		PinReaderShell::~PinReaderShell()
		{
			stop();
			while (isRunning()) {/*Blocks*/ }
			std::lock_guard<std::recursive_mutex> lock(mutex);
		}

		void PinReaderShell::setActive(const bool isActive)
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

		void PinReaderShell::setPin(const gpio::Pin newPin)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (pin != newPin)
			{
				se::log::info("Setting pin reader pin to: " + std::to_string(gpio::getPinEnumAsNumber(newPin)));
				pin = newPin;
				clearHistory = true;
				history.clear();
			}
		}

		void PinReaderShell::syncCreate(WriteBuffer& buffer)
		{

		}

		void PinReaderShell::syncCreate(ReadBuffer& buffer)
		{

		}

		void PinReaderShell::syncRemove(WriteBuffer& buffer)
		{

		}

		void PinReaderShell::syncRemove(ReadBuffer& buffer)
		{
			stop();
			while (isRunning()) {}
		}

		bool PinReaderShell::syncUpdate(const se::time::Time deltaTime)
		{
			if (sizeof(size_t) + history.size() * (sizeof(PinReaderHistoryEntry::state) + sizeof(PinReaderHistoryEntry::time)) > 1400)
				return true;//Default MTU is around 1500, preferably keep it below that
			return false;
		}

		void PinReaderShell::syncUpdate(WriteBuffer& buffer)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			buffer.write(clearHistory);
			clearHistory = false;
			buffer.write(history);
			history.clear();
		}

		void PinReaderShell::syncUpdate(ReadBuffer& buffer)
		{
			bool _active;
			buffer.read(_active);
			setActive(_active);
			gpio::Pin _pin;
			buffer.read(_pin);
			setPin(_pin);
		}

		void PinReaderShell::getHistory(PinReaderHistory& deposit) const
		{
			deposit = history;
		}

		void PinReaderShell::onStart()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			history.clear();
		}

		void PinReaderShell::update()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			const gpio::PinState state = gpio::read(pin);
			if (history.empty() || state != history.back().state)
			{
				history.push_back(PinReaderHistoryEntry(state, se::time::now()));
			}
		}

		void PinReaderShell::onStop()
		{
			
		}
	}
}