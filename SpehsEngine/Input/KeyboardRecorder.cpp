#include "stdafx.h"
#include "SpehsEngine/Input/KeyboardRecorder.h"

#include "SpehsEngine/Input/EventSignaler.h"
#include "SpehsEngine/Input/KeyUtilityFunctions.h"


namespace se::input
{
	KeyboardRecorder::KeyboardRecorder(EventSignaler& _eventSignaler, const int _inputPriority, const std::string_view _string)
		: output32(_string.begin(), _string.end())
		, cursorIndex(_string.length())
	{
		_eventSignaler.connectToKeyboardSignal(keyboardEventConnection,
			[this](const KeyboardEvent& _event)
			{
				constexpr char32_t whitespace = ' ';
				if (_event.isPress())
				{
					if ((_event.key == Key::RETURN) || (_event.key == Key::ESCAPE))
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
									const bool isWhitespace = output32[removeIndex] == whitespace;
									if (isWhitespace && removeCount > 0)
									{
										// Already removed something and encountered whitespace -> stop
										break;
									}
									output32.erase(removeIndex, 1);
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
								output32.erase(cursorIndex - 1, 1);
								cursorIndex--;
							}
						}
						return true;
					}
					else if (_event.key == Key::KEY_DELETE)
					{
						if (cursorIndex < output32.length())
						{
							if (isCtrlDown())
							{
								size_t removeCount = 0;
								while (true)
								{
									const size_t removeIndex = cursorIndex;
									const bool isWhitespace = output32[removeIndex] == whitespace;
									if (isWhitespace && removeCount > 0)
									{
										// Already removed something and encountered whitespace -> stop
										break;
									}
									output32.erase(removeIndex, 1);
									removeCount++;
									if (cursorIndex == output32.length())
									{
										break;
									}
								}
							}
							else
							{
								output32.erase(cursorIndex, 1);
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
						if (cursorIndex < output32.length())
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
						cursorIndex = output32.length();
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
					for (const char32_t c32 : _event.string)
					{
						if (isValidFunction)
						{
							if (!isValidFunction(c32))
							{
								continue;
							}
						}
						output32 += c32;
						cursorIndex++;
					}
				}
				return true;
			}, _inputPriority);
	}

	std::optional<std::string> KeyboardRecorder::getResult() const
	{
		if (finished)
		{
			return UTF32::toUTF8(output32);
		}
		else
		{
			return std::nullopt;
		}
	}
}
