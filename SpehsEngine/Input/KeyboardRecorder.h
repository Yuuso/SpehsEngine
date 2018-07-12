#pragma once
#include <vector>
#include "SpehsEngine/Core/Time.h"

namespace se
{
	class InputManager;

	/** This class can be used to record text input from the keyboard.
	All input is divided into two types: character and command input.
	Registered keystrokes can be obtained from the input vectors after the update.

	A default list of most commonly used command keys is listened to, but the list can be altered by the user.

	The purpose of this class is to provide a layer between the input manager and an upper layer entity, which is responsible for processing the command key strokes.
	*/
	class KeyboardRecorder
	{
	public:
		KeyboardRecorder(InputManager& inputManager);
		virtual ~KeyboardRecorder();

		virtual void update(const time::Time deltaTime);
		void stop();///< Resets the recorder so that all ongoing recording is stopped
		void setBeginKeyRepeatTimer(const float seconds);///< Time after the first keystroke of a pressed key to begin receiving keystrokes continuously
		void setContinuousKeyRepeatTimer(const float seconds);///< Time between continuous keystrokes

		////Command keys
		void addCommandKey(const unsigned key);///< Lists specified key as a command key, and will register keystrokes from that kay into the command input vector
		void removeCommandKey(const unsigned key);///< Removes specified key from the list of listened command keys
		void clearCommandKeys();///< Removes all command keys currently listed as listened keys
		bool isCommandKey(const unsigned key) const;///< Returns whether specified key is listened as a command key
		std::vector<unsigned> getCommandKeys() const { return commandKeys; }

		InputManager& inputManager;

		//Registered keystroked will appear in these vector after the update
		std::vector<char> characterInput;
		std::vector<unsigned> commandInput;//The value indicates key code of the "command" key
	private:
		bool tryKey(unsigned key);//Checks whether key should be registered as a keystroke
		struct KeyHoldData
		{
			unsigned key;//Key ID
			time::Time timer;//Timer until re-press
		};
		std::vector<KeyHoldData> heldKeys;
		std::vector<unsigned> commandKeys;
		time::Time beginKeyRepeatTime;
		time::Time continueKeyRepeatTime;
	};
}