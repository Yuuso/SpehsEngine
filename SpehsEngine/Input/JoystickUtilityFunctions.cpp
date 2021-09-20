#include "stdafx.h"
#include "SpehsEngine/Input/JoystickUtilityFunctions.h"

#include "SpehsEngine/Core/Archive.h"
#include "SDL/SDL_joystick.h"


namespace se
{
	namespace input
	{
		namespace
		{
			SDL_Joystick* getJoystickSdl(const JoystickId joystickId)
			{
				const SDL_JoystickID joystickIdSdl = SDL_JoystickID(joystickId.value);
				return SDL_JoystickFromInstanceID(joystickIdSdl);
			}

			JoystickId getJoystickId(SDL_Joystick& joystickSdl)
			{
				return JoystickId(SDL_JoystickInstanceID(&joystickSdl));
			}
		}

		bool getJoystickConnected(const JoystickId joystickId)
		{
			if (SDL_Joystick* const joystickSdl = getJoystickSdl(joystickId))
			{
				return SDL_JoystickGetAttached(joystickSdl) == SDL_TRUE;
			}
			return false;
		}

		const char* getJoystickName(const JoystickId joystickId)
		{
			SDL_Joystick* const joystickSdl = getJoystickSdl(joystickId);
			se_assert(joystickSdl);
			if (joystickSdl)
			{
				return SDL_JoystickName(joystickSdl);
			}
			return nullptr;
		}

		JoystickType getJoystickType(const JoystickId joystickId)
		{
			SDL_Joystick* const joystickSdl = getJoystickSdl(joystickId);
			se_assert(joystickSdl);
			if (joystickSdl)
			{
				switch (SDL_JoystickGetType(joystickSdl))
				{
				case SDL_JOYSTICK_TYPE_UNKNOWN: return JoystickType::Unknown;
				case SDL_JOYSTICK_TYPE_GAMECONTROLLER: return JoystickType::GameController;
				case SDL_JOYSTICK_TYPE_WHEEL: return JoystickType::Wheel;
				case SDL_JOYSTICK_TYPE_ARCADE_STICK: return JoystickType::ArcadeStick;
				case SDL_JOYSTICK_TYPE_FLIGHT_STICK: return JoystickType::FlightStick;
				case SDL_JOYSTICK_TYPE_DANCE_PAD: return JoystickType::DancePad;
				case SDL_JOYSTICK_TYPE_GUITAR: return JoystickType::Guitar;
				case SDL_JOYSTICK_TYPE_DRUM_KIT: return JoystickType::DrumKit;
				case SDL_JOYSTICK_TYPE_ARCADE_PAD: return JoystickType::ArcadePad;
				case SDL_JOYSTICK_TYPE_THROTTLE: return JoystickType::Throttle;
				}
			}
			return JoystickType::None;
		}

		JoystickGuid getJoystickGuid(const JoystickId joystickId)
		{
			SDL_Joystick* const joystickSdl = getJoystickSdl(joystickId);
			se_assert(joystickSdl);
			if (joystickSdl)
			{
				SDL_JoystickGUID joystickGuidSdl = SDL_JoystickGetGUID(joystickSdl);
				JoystickGuid joystickGuid;
				memcpy(joystickGuid.data, joystickGuidSdl.data, 16);
				return joystickGuid;
			}
			return JoystickGuid();
		}

		uint8_t getJoystickButtonCount(const JoystickId joystickId)
		{
			SDL_Joystick* const joystickSdl = getJoystickSdl(joystickId);
			se_assert(joystickSdl);
			if (joystickSdl)
			{
				return uint8_t(SDL_JoystickNumAxes(joystickSdl));
			}
			return 0;
		}

		uint8_t getJoystickAxisCount(const JoystickId joystickId)
		{
			SDL_Joystick* const joystickSdl = getJoystickSdl(joystickId);
			se_assert(joystickSdl);
			if (joystickSdl)
			{
				return uint8_t(SDL_JoystickNumButtons(joystickSdl));
			}
			return 0;
		}

		uint8_t getJoystickHatCount(const JoystickId joystickId)
		{
			SDL_Joystick* const joystickSdl = getJoystickSdl(joystickId);
			se_assert(joystickSdl);
			if (joystickSdl)
			{
				return uint8_t(SDL_JoystickNumHats(joystickSdl));
			}
			return 0;
		}

		bool getJoystickButtonState(const JoystickId joystickId, const uint8_t buttonIndex)
		{
			SDL_Joystick* const joystickSdl = getJoystickSdl(joystickId);
			se_assert(joystickSdl);
			if (joystickSdl)
			{
				return SDL_JoystickGetButton(joystickSdl, int(buttonIndex)) != 0; // TODO: verify return value
			}
			return false;
		}

		int16_t getJoystickAxis(const JoystickId joystickId, const uint8_t axisIndex)
		{
			SDL_Joystick* const joystickSdl = getJoystickSdl(joystickId);
			se_assert(joystickSdl);
			if (joystickSdl)
			{
				return SDL_JoystickGetAxis(joystickSdl, int(axisIndex));
			}
			return 0;
		}

		JoystickHatState getJoystickHatState(const JoystickId joystickId, const uint8_t hatIndex)
		{
			SDL_Joystick* const joystickSdl = getJoystickSdl(joystickId);
			se_assert(joystickSdl);
			if (joystickSdl)
			{
				return JoystickHatState(SDL_JoystickGetHat(joystickSdl, int(hatIndex)));
			}
			return JoystickHatState::Center;
		}

		bool setJoystickRumble(const JoystickId joystickId, const uint16_t lowFrequency, const uint16_t highFrequency, const time::Time duration)
		{
			SDL_Joystick* const joystickSdl = getJoystickSdl(joystickId);
			se_assert(joystickSdl);
			if (joystickSdl)
			{
				return SDL_JoystickRumble(joystickSdl, lowFrequency, highFrequency, uint32_t(duration.asMilliseconds())) == 0;
			}
			return false;
		}

		Archive writeToArchive(const input::JoystickGuid& joystickGuid)
		{
			Archive archive;
			uint64_t data1 = 0;
			uint64_t data2 = 0;
			memcpy(&data1, &joystickGuid.data[0], 8);
			memcpy(&data2, &joystickGuid.data[8], 8);
			se_write_to_archive(archive, data1);
			se_write_to_archive(archive, data2);
			return archive;
		}

		bool readFromArchive(const Archive& archive, input::JoystickGuid& joystickGuid)
		{
			uint64_t data1 = 0;
			uint64_t data2 = 0;
			se_read_from_archive(archive, data1);
			se_read_from_archive(archive, data2);
			memcpy(&joystickGuid.data[0], &data1, 8);
			memcpy(&joystickGuid.data[8], &data2, 8);
			return true;
		}
	}
}
