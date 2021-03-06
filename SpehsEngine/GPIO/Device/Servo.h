#pragma once
#include <thread>
#include <mutex>
#include <iostream>
#include <bcm2835.h>
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/GPIO/Device/ThreadedDevice.h"
#include "SpehsEngine/GPIO/Pin.h"
#include "SpehsEngine/Sync/ISyncType.h"



namespace se
{
	namespace device
	{
		class IServo : public sync::IType
		{
		public:
			
			//Initialization
			/* Sets the pin that the servo is connected to. */
			virtual void setPin(const gpio::Pin pin) = 0;
			/* Set the pulse durations used to set the servo to its minimum and maximum angles. */
			virtual void setAngleLimits(const se::time::Time minPulseWidth, const se::time::Time maxPulseWidth, const float minAngle, const float maxAngle) = 0;
			virtual void setMinAngle(const se::time::Time minPulseWidth, const float minAngle) = 0;
			virtual void setMaxAngle(const se::time::Time maxPulseWidth, const float maxAngle) = 0;
			/* Set a rotation speed per second approximation. */
			virtual void setRotationSpeed(const float speed) = 0;

			//Control
			virtual void setActive(const bool isActive) = 0;
			virtual void setTargetAngle(const float angle) = 0;

			//State query
			virtual gpio::Pin getPin() const = 0;
			virtual float getApproximatedAngle() const = 0;
			virtual float getRotationSpeed() const = 0;
			virtual float getMinAngle() const = 0;
			virtual float getMaxAngle() const = 0;
		};



		class ServoGhost final : public IServo
		{
			SPEHS_SYNC_TYPE_DECL(ServoGhost, 0)
		public:
			ServoGhost();
			~ServoGhost();
			
			//Servo interface
			void setPin(const gpio::Pin pin) override;
			void setAngleLimits(const se::time::Time minPulseWidth, const se::time::Time maxPulseWidth, const float minAngle, const float maxAngle) override;
			void setMinAngle(const se::time::Time minPulseWidth, const float minAngle) override;
			void setMaxAngle(const se::time::Time maxPulseWidth, const float maxAngle) override;
			void setRotationSpeed(const float speed) override;
			void setActive(const bool isActive) override;
			void setTargetAngle(const float angle) override;
			gpio::Pin getPin() const override;
			float getApproximatedAngle() const override;
			float getRotationSpeed() const override;
			float getMinAngle() const override;
			float getMaxAngle() const override;

			//Sync type
			void syncCreate(WriteBuffer& buffer) override;
			void syncCreate(ReadBuffer& buffer) override;
			void syncRemove(WriteBuffer& buffer) override;
			void syncRemove(ReadBuffer& buffer) override;
			bool syncUpdate(const se::time::Time deltaTime) override;
			void syncUpdate(WriteBuffer& buffer) override;
			void syncUpdate(ReadBuffer& buffer) override;

			std::string name;

		private:
			float targetAngle;
			float approximatedAngle;
			gpio::Pin pin;
			se::time::Time minPulseWidth;
			se::time::Time maxPulseWidth;
			float rotationSpeed;
			float minAngle;
			float maxAngle;
			bool active;

			bool syncRequested;
		};
		


		class ServoShell final : public ThreadedDevice, public IServo
		{
			SPEHS_SYNC_TYPE_DECL(ServoShell, 0)
		public:
			ServoShell();
			~ServoShell();

			//Servo interface
			void setPin(const gpio::Pin pin) override;
			void setAngleLimits(const se::time::Time minPulseWidth, const se::time::Time maxPulseWidth, const float minAngle, const float maxAngle) override;
			void setMinAngle(const se::time::Time minPulseWidth, const float minAngle) override;
			void setMaxAngle(const se::time::Time maxPulseWidth, const float maxAngle) override;
			void setRotationSpeed(const float speed) override;
			void setActive(const bool isActive) override;
			void setTargetAngle(const float angle) override;
			gpio::Pin getPin() const override;
			float getApproximatedAngle() const override;
			float getRotationSpeed() const override;
			float getMinAngle() const override;
			float getMaxAngle() const override;
			
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

			mutable std::recursive_mutex mutex;
			se::time::Time lastUpdateTime;
			float targetAngle;
			float approximatedAngle;
			gpio::Pin pin;
			se::time::Time minPulseWidth;
			se::time::Time maxPulseWidth;
			float rotationSpeed;
			float minAngle;
			float maxAngle;
			bool active;

			se::time::Time updateTimer;
		};
	}
}