#pragma once
#include <mutex>
#include <vector>
#include <thread>
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/GPIO/Device/ThreadedDevice.h"
#include "SpehsEngine/GPIO/Device/PinReader.h"
#include "SpehsEngine/GPIO/Pin.h"
#include "SpehsEngine/Sync/ISyncType.h"

namespace spehs
{
	namespace net
	{
		class WriteBuffer;
		class ReadBuffer;
	}

	namespace device
	{
		class PWMHistory : public std::vector<float>
		{
		public:
			void write(net::WriteBuffer& buffer) const;
			void read(net::ReadBuffer& buffer);
		};

		class PinReaderPWMGhost : public sync::IType
		{
			SPEHS_SYNC_TYPE_DECL(PinReaderPWMGhost, 0)
		public:
			PinReaderPWMGhost();
			~PinReaderPWMGhost();

			//Sync type
			void syncCreate(net::WriteBuffer& buffer) override;
			void syncCreate(net::ReadBuffer& buffer) override;
			void syncRemove(net::WriteBuffer& buffer) override;
			void syncRemove(net::ReadBuffer& buffer) override;
			bool syncUpdate(const spehs::time::Time deltaTime) override;
			void syncUpdate(net::WriteBuffer& buffer) override;
			void syncUpdate(net::ReadBuffer& buffer) override;

			void setPin(const gpio::Pin pin);
			void setActive(const bool isActive);
			void setSampleSize(const size_t size);
			void setSampleRate(const spehs::time::Time interval);
			void clearHistory();

			const PWMHistory& getHistory() const { return history; }
			const gpio::Pin getPin() const { return pin; }

			std::string name;

		private:
			PWMHistory history;
			bool active;
			bool requestUpdate;
			gpio::Pin pin;
			size_t sampleSize;
			spehs::time::Time sampleRate;
		};

		class PinReaderPWMShell final : public ThreadedDevice, public sync::IType
		{
			SPEHS_SYNC_TYPE_DECL(PinReaderPWMShell, 0)
		public:
			PinReaderPWMShell();
			~PinReaderPWMShell();

			void setActive(const bool isActive);
			void setPin(const gpio::Pin pin);
			void setSampleSize(const size_t size);
			void setSampleRate(const spehs::time::Time interval);
			void getHistory(PWMHistory& deposit) const;

			//Sync type
			void syncCreate(net::WriteBuffer& buffer) override;
			void syncCreate(net::ReadBuffer& buffer) override;
			void syncRemove(net::WriteBuffer& buffer) override;
			void syncRemove(net::ReadBuffer& buffer) override;
			bool syncUpdate(const spehs::time::Time deltaTime) override;
			void syncUpdate(net::WriteBuffer& buffer) override;
			void syncUpdate(net::ReadBuffer& buffer) override;
			
		private:
			//Threaded device
			void onStart() override;
			void update() override;
			void onStop() override;

			std::recursive_mutex mutex;
			PWMHistory history;
			bool active;
			bool requestUpdate;
			bool clearHistory;
			gpio::Pin pin;
			size_t highStateSampleCount;
			size_t lowStateSampleCount;
			size_t sampleSize;
			spehs::time::Time sampleRate;
			spehs::time::Time lastReadTime;
		};
	}
}