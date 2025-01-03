#include "stdafx.h"

#include "SpehsEngine/Input/KeyboardRecorder.h"
#include "SpehsEngine/Input/InputManager.h"


namespace se
{
	namespace input
	{
		KeyboardRecorder::KeyboardRecorder(InputManager& _inputManager)
			: inputManager(_inputManager)
			, beginKeyRepeatTime(Time::fromSeconds(0.66f))
			, continueKeyRepeatTime(Time::fromSeconds(0.1f))
		{
			commandKeys.push_back(KEYBOARD_RETURN);
			commandKeys.push_back(KEYBOARD_KP_ENTER);
			commandKeys.push_back(KEYBOARD_BACKSPACE);
			commandKeys.push_back(KEYBOARD_DELETE);
			commandKeys.push_back(KEYBOARD_LEFT);
			commandKeys.push_back(KEYBOARD_RIGHT);
			commandKeys.push_back(KEYBOARD_UP);
			commandKeys.push_back(KEYBOARD_DOWN);
			commandKeys.push_back(KEYBOARD_HOME);
			commandKeys.push_back(KEYBOARD_END);
			commandKeys.push_back(KEYBOARD_PAGEUP);
			commandKeys.push_back(KEYBOARD_PAGEDOWN);
			commandKeys.push_back(KEYBOARD_TAB);
			commandKeys.push_back(KEYBOARD_ESCAPE);
		}

		KeyboardRecorder::~KeyboardRecorder()
		{

		}

		void KeyboardRecorder::stop()
		{
			heldKeys.clear();
			characterInput.clear();
			commandInput.clear();
		}

		void KeyboardRecorder::setBeginKeyRepeatTimer(const float seconds)
		{
			beginKeyRepeatTime = Time::fromSeconds(seconds);
			for (unsigned i = 0; i < heldKeys.size(); i++)
				heldKeys[i].timer = std::min(Time::fromSeconds(seconds), heldKeys[i].timer);
		}

		void KeyboardRecorder::setContinuousKeyRepeatTimer(const float seconds)
		{
			continueKeyRepeatTime = Time::fromSeconds(seconds);
			for (unsigned i = 0; i < heldKeys.size(); i++)
				heldKeys[i].timer = std::min(Time::fromSeconds(seconds), heldKeys[i].timer);
		}

		void KeyboardRecorder::addCommandKey(const unsigned key)
		{
			for (unsigned i = 0; i < commandKeys.size(); i++)
			{
				if (commandKeys[i] == key)
					return;
			}
			commandKeys.push_back(key);
		}

		void KeyboardRecorder::removeCommandKey(const unsigned key)
		{
			for (unsigned i = 0; i < commandKeys.size(); i++)
			{
				if (commandKeys[i] == key)
				{
					commandKeys.erase(commandKeys.begin() + i);
					return;
				}
			}
		}

		void KeyboardRecorder::clearCommandKeys()
		{
			commandKeys.clear();
		}

		bool KeyboardRecorder::isCommandKey(const unsigned key) const
		{
			for (unsigned i = 0; i < commandKeys.size(); i++)
			{
				if (commandKeys[i] == key)
					return true;
			}
			return false;
		}

		bool KeyboardRecorder::tryKey(unsigned key)
		{
			if (!inputManager.isKeyDown(key))
				return false;
			for (unsigned i = 0; i < heldKeys.size(); i++)
			{
				if (heldKeys[i].key == key)
				{
					if (heldKeys[i].timer <= Time::zero)
					{//Ready to re-register key stroke
						heldKeys[i].timer = continueKeyRepeatTime;//Repeated strike timer
						return true;
					}
					else
						return false;//Timer still ticking
				}
			}
			heldKeys.resize(heldKeys.size() + 1);
			heldKeys.back().key = key;
			heldKeys.back().timer = beginKeyRepeatTime;//First stroke
			return true;
		}

		void KeyboardRecorder::update(const Time time)
		{
			characterInput.clear();
			commandInput.clear();
			const bool ctrlDown = inputManager.isKeyDown(KEYBOARD_LCTRL) || inputManager.isKeyDown(KEYBOARD_RCTRL);
			const bool shiftDown = inputManager.isKeyDown(KEYBOARD_LSHIFT) || inputManager.isKeyDown(KEYBOARD_RSHIFT);

			//Decrease held key timers - if still held
			for (unsigned i = 0; i < heldKeys.size();)
			{
				if (!inputManager.isKeyDown(heldKeys[i].key))
					heldKeys.erase(heldKeys.begin() + i);
				else
				{
					heldKeys[i].timer -= time;
					i++;
				}
			}

			////CHARACTER INPUT
			if (!ctrlDown)
			{
				//Alphabet
				int capital = 0;
				if (shiftDown)
					capital = -32;
				for (unsigned i = 97; i <= 122; i++)
				{
					if (tryKey(i))
						characterInput.push_back(char(i + unsigned(capital)));
				}

				//Numbers
				for (int i = 48; i <= 57; i++)
				{
					if (tryKey(i))
					{
						if (shiftDown)
							characterInput.push_back(char(i - 10));
						else
							characterInput.push_back(char(i));
					}
				}
				for (int i = KEYBOARD_KP_1; i <= KEYBOARD_KP_9; i++)
				{//Keypad numbers 1-9
					if (tryKey(i))
						characterInput.push_back(char(i - KEYBOARD_KP_1 + 49));
				}
				//Keypad 0 requires aseparate call as it is aligned after 9 in the engine key enum, but before 1 in ASCII...
				if (tryKey(KEYBOARD_KP_0))
					characterInput.push_back(char('0'));
			}
			//Other characters
			if (tryKey(KEYBOARD_SPACE))
				characterInput.push_back(' ');
			if (tryKey(KEYBOARD_COMMA))
				characterInput.push_back(',');
			if (tryKey(KEYBOARD_KP_PERIOD) || tryKey(KEYBOARD_PERIOD))
				characterInput.push_back('.');
			if (tryKey(KEYBOARD_KP_MINUS) || tryKey(KEYBOARD_MINUS))
				characterInput.push_back('-');
			if (tryKey(KEYBOARD_AMPERSAND))
				characterInput.push_back('&');
			if (tryKey(KEYBOARD_SLASH))
				characterInput.push_back('/');
			if (tryKey(KEYBOARD_QUESTION))
				characterInput.push_back('?');
			if (tryKey(KEYBOARD_EXCLAIM))
				characterInput.push_back('!');
			if (tryKey(KEYBOARD_EQUALS))
				characterInput.push_back('=');

			//COMMAND INPUT
			for (unsigned i = 0; i < commandKeys.size(); i++)
			{
				if (tryKey(commandKeys[i]))
					commandInput.push_back(commandKeys[i]);
			}

			//Clean up held keys that were not re-stroke
			for (unsigned i = 0; i < heldKeys.size();)
			{
				if (heldKeys[i].timer <= Time::zero)
					heldKeys.erase(heldKeys.begin() + i);
				else
					i++;
			}
		}
	}
}