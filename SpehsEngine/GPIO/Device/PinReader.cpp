#include <cstring>
#include "SpehsEngine/GPIO/Device/PinReader.h"
#include "SpehsEngine/Net/Protocol.h"

namespace spehs
{
	namespace device
	{
		void PinReaderHistoryEntry::write(net::WriteBuffer& buffer) const
		{
			buffer.write(state);
			buffer.write(time);
		}

		void PinReaderHistoryEntry::read(net::ReadBuffer& buffer)
		{
			buffer.read(state);
			buffer.read(time);
		}

		void PinReaderHistory::write(net::WriteBuffer& buffer) const
		{
			const size_t count = size();//TODO: buffer.write(std::vector<...>)
			buffer.write(count);
			for (size_t i = 0; i < count; i++)
				buffer.write(at(i));
		}

		void PinReaderHistory::read(net::ReadBuffer& buffer)
		{
			size_t count;
			buffer.read(count);//TODO: buffer.read(std::vector<...>)
			resize(count);
			for (size_t i = 0; i < count; i++)
				buffer.read(at(i));
		}

		gpio::PinState PinReaderHistory::stateAtTime(const spehs::time::Time& time) const
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

		void PinReaderGhost::syncCreate(net::WriteBuffer& buffer)
		{

		}

		void PinReaderGhost::syncCreate(net::ReadBuffer& buffer)
		{

		}

		void PinReaderGhost::syncRemove(net::WriteBuffer& buffer)
		{

		}

		void PinReaderGhost::syncRemove(net::ReadBuffer& buffer)
		{

		}

		bool PinReaderGhost::syncUpdate(const spehs::time::Time deltaTime)
		{
			return requestUpdate;
		}

		void PinReaderGhost::syncUpdate(net::WriteBuffer& buffer)
		{
			buffer.write(active);
			buffer.write(pin);
			requestUpdate = false;
		}

		void PinReaderGhost::syncUpdate(net::ReadBuffer& buffer)
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
				spehs::log::info("Setting pin reader pin to: " + std::to_string(gpio::getPinEnumAsNumber(newPin)));
				pin = newPin;
				clearHistory = true;
				history.clear();
			}
		}

		void PinReaderShell::syncCreate(net::WriteBuffer& buffer)
		{

		}

		void PinReaderShell::syncCreate(net::ReadBuffer& buffer)
		{

		}

		void PinReaderShell::syncRemove(net::WriteBuffer& buffer)
		{

		}

		void PinReaderShell::syncRemove(net::ReadBuffer& buffer)
		{
			stop();
			while (isRunning()) {}
		}

		bool PinReaderShell::syncUpdate(const spehs::time::Time deltaTime)
		{
			if (sizeof(size_t) + history.size() * (sizeof(PinReaderHistoryEntry::state) + sizeof(PinReaderHistoryEntry::time)) > 1400)
				return true;//Default MTU is around 1500, preferably keep it below that
			return false;
		}

		void PinReaderShell::syncUpdate(net::WriteBuffer& buffer)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			buffer.write(clearHistory);
			clearHistory = false;
			buffer.write(history);
			history.clear();
		}

		void PinReaderShell::syncUpdate(net::ReadBuffer& buffer)
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
				history.push_back(PinReaderHistoryEntry(state, spehs::time::now()));
			}
		}

		void PinReaderShell::onStop()
		{
			
		}
	}
}