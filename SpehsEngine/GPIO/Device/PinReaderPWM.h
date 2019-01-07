#pragma once
#include <mutex>
#include <vector>
#include <thread>
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/GPIO/Device/ThreadedDevice.h"
#include "SpehsEngine/GPIO/Device/PinReader.h"
#include "SpehsEngine/GPIO/Pin.h"
#include "SpehsEngine/Sync/ISyncType.h"

namespace se
{
	class WriteBuffer;
	class ReadBuffer;

	namespace device
	{
		class PWMHistory : public std::vector<float>
		{
		public:
			void write(WriteBuffer& buffer) const;
			bool read(ReadBuffer& buffer);
		};

		class PinReaderPWMGhost : public sync::IType
		{
			SPEHS_SYNC_TYPE_DECL(PinReaderPWMGhost, 0)
		public:
			PinReaderPWMGhost();
			~PinReaderPWMGhost();

			//Sync type
			void syncCreate(WriteBuffer& buffer) override;
			void syncCreate(ReadBuffer& buffer) override;
			void syncRemove(WriteBuffer& buffer) override;
			void syncRemove(ReadBuffer& buffer) override;
			bool syncUpdate(const se::time::Time deltaTime) override;
			void syncUpdate(WriteBuffer& buffer) override;
			void syncUpdate(ReadBuffer& buffer) override;

			void setPin(const gpio::Pin pin);
			void setActive(const bool isActive);
			void setSampleSize(const size_t size);
			void setSampleRate(const se::time::Time interval);
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
			se::time::Time sampleRate;
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
			void setSampleRate(const se::time::Time interval);
			void getHistory(PWMHistory& deposit) const;

			//Sync type
			void syncCreate(WriteBuffer& buffer) override;
			void syncCreate(ReadBuffer& buffer) override;
			void syncRemove(WriteBuffer& buffer) override;
			void syncRemove(ReadBuffer& buffer) override;
			bool syncUpdate(const se::time::Time deltaTime) override;
			void syncUpdate(WriteBuffer& buffer) override;
			void syncUpdate(ReadBuffer& buffer) override;
			
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
			se::time::Time sampleRate;
			se::time::Time lastReadTime;
		};
	}
}