#pragma once

#include "SpehsEngine/Core/Archive.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Input/JoystickId.h"
#include "SpehsEngine/Input/JoystickGuid.h"
#include "SpehsEngine/Input/JoystickHatState.h"


namespace se
{
	namespace input
	{
		enum class JoystickType : uint8_t
		{
            None,
            Unknown,
            GameController,
            Wheel,
            ArcadeStick,
            FlightStick,
            DancePad,
            Guitar,
            DrumKit,
            ArcadePad,
            Throttle,
		};

        bool getJoystickConnected(const JoystickId joystickId);
        const char* getJoystickName(const JoystickId joystickId);
        JoystickType getJoystickType(const JoystickId joystickId);
        JoystickGuid getJoystickGuid(const JoystickId joystickId);
        uint8_t getJoystickButtonCount(const JoystickId joystickId);
        uint8_t getJoystickAxisCount(const JoystickId joystickId);
        uint8_t getJoystickHatCount(const JoystickId joystickId);
        bool getJoystickButtonState(const JoystickId joystickId, const uint8_t buttonIndex);
        int16_t getJoystickAxis(const JoystickId joystickId, const uint8_t axisIndex);
        JoystickHatState getJoystickHatState(const JoystickId joystickId, const uint8_t hatIndex);

        bool setJoystickRumble(const JoystickId joystickId, const uint16_t lowFrequency, const uint16_t highFrequency, const time::Time duration);

        Archive writeToArchive(const JoystickGuid& joystickGuid);
        bool readFromArchive(const Archive& archive, JoystickGuid& joystickGuid);
	}
}
