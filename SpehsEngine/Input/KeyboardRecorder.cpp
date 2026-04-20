#include "stdafx.h"
#include "SpehsEngine/Input/KeyboardRecorder.h"

#include "SpehsEngine/Input/EventSignaler.h"
#include "SpehsEngine/Input/KeyUtilityFunctions.h"


namespace se::input
{
	KeyboardRecorder::KeyboardRecorder(EventSignaler& _eventSignaler, const int _inputPriority, const std::u32string_view _string)
		: output(_string)
		, cursorIndex(_string.length())
	{
		_eventSignaler.connectToKeyboardSignal(keyboardEventConnection,
			[this](const KeyboardEvent& _event)
			{
				constexpr char32_t whitespace = ' ';
				if (_event.isPress())
				{
					if (_event.key == Key::RETURN)
					{
						finished = true;
						return true;
					}
					else if (_event.key == Key::BACKSPACE)
					{
						if (cursorIndex > 0)
						{
							if (isCtrlDown())
							{
								size_t removeCount = 0;
								while (true)
								{
									const size_t removeIndex = cursorIndex - 1;
									const bool isWhitespace = output[removeIndex] == whitespace;
									if (isWhitespace && removeCount > 0)
									{
										// Already removed something and encountered whitespace -> stop
										break;
									}
									output.erase(removeIndex, 1);
									cursorIndex--;
									removeCount++;
									if (cursorIndex == 0)
									{
										break;
									}
								}
							}
							else
							{
								output.erase(cursorIndex - 1, 1);
								cursorIndex--;
							}
						}
						return true;
					}
					else if (_event.key == Key::KEY_DELETE)
					{
						if (cursorIndex < output.length())
						{
							if (isCtrlDown())
							{
								size_t removeCount = 0;
								while (true)
								{
									const size_t removeIndex = cursorIndex;
									const bool isWhitespace = output[removeIndex] == whitespace;
									if (isWhitespace && removeCount > 0)
									{
										// Already removed something and encountered whitespace -> stop
										break;
									}
									output.erase(removeIndex, 1);
									removeCount++;
									if (cursorIndex == output.length())
									{
										break;
									}
								}
							}
							else
							{
								output.erase(cursorIndex, 1);
							}
						}
						return true;
					}
					else if (_event.key == Key::LEFT)
					{
						if (cursorIndex > 0)
						{
							cursorIndex--;
						}
						return true;
					}
					else if (_event.key == Key::RIGHT)
					{
						if (cursorIndex < output.length())
						{
							cursorIndex++;
						}
						return true;
					}
					else if (_event.key == Key::HOME)
					{
						cursorIndex = 0;
						return true;
					}
					else if (_event.key == Key::END)
					{
						cursorIndex = output.length();
						return true;
					}
				}
				return false;
			}, _inputPriority);

		_eventSignaler.connectToTextInputSignal(textInputConnection,
			[this](const TextInputEvent& _event)
			{
				if (_event.string.empty())
				{
					se_assert(false && "Unexpected empty text");
				}
				else
				{
					output += _event.string;
					cursorIndex++;
				}
				return true;
			}, _inputPriority);
	}
}
