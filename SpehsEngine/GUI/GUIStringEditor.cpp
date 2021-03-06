#include "stdafx.h"
#include <algorithm>
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/OS.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/DeltaTimeSystem.h"
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/GUI/GUIStringEditor.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/Text.h"


namespace se
{
	size_t GUIStringEditor::defaultMaxStringEditorStringLength = 32;

	GUIStringEditor::GUIStringEditor(GUIContext& context, const std::string& initialValue)
		: GUIRectangle(context)
		, ValueEditor(initialValue)
		, maxStringLength(defaultMaxStringEditorStringLength)
		, keyboardRecorder(context.getInputManager())
	{
		setTyperBlinkTime(time::fromSeconds(0.5f));
		createText();
		typeCharacter = getBatchManager().createText("|", getDepth() + 1);
		typeCharacter->setRenderState(getRenderState() && isReceivingInput());
		typeCharacter->setFont("Fonts/Anonymous.ttf", 12);
		typeCharacter->setColor(text->getColor());
		onEditorValueChange();
	}

	GUIStringEditor::~GUIStringEditor()
	{
		typeCharacter->destroy();
	}

	void GUIStringEditor::setRenderState(const bool _state)
	{
		GUIRectangle::setRenderState(_state);
		typeCharacter->setRenderState(_state && isReceivingInput());
	}

	void GUIStringEditor::setDepth(const int16_t depth)
	{
		GUIRectangle::setDepth(depth);
		typeCharacter->setPlaneDepth(depth + 1);
	}

	void GUIStringEditor::setStringSize(const int _size)
	{
		GUIRectangle::setStringSize(_size);
		typeCharacter->setFontSize(_size);
	}

	void GUIStringEditor::setStringSizeRelative(const int relativeSize)
	{
		GUIRectangle::setStringSizeRelative(relativeSize);
		typeCharacter->setFontSize(12 + relativeSize);
	}

	void GUIStringEditor::setStringColor(const Color& col)
	{
		GUIRectangle::setStringColor(col);
		typeCharacter->setColor(col);
	}

	void GUIStringEditor::setStringAlpha(const float a)
	{
		GUIRectangle::setStringAlpha(a);
		typeCharacter->setAlpha(a);
	}

	void GUIStringEditor::updatePosition()
	{
		GUIRectangle::updatePosition();
		if (text->getString().size() > 0)
			typeCharacter->setPosition(text->getX(typerPosition) - typeCharacter->getTextWidth() * 0.5f, text->getY(typerPosition));
		else//No characters written
			typeCharacter->setPosition(text->getX(typerPosition) - typeCharacter->getTextWidth() * 0.5f, text->getY() - 0.5f * text->getFontHeight());
	}

	void GUIStringEditor::updateMinSize()
	{
		GUIRectangle::updateMinSize();
		minSize.x += int(typeCharacter->getTextWidth());
	}

	void GUIStringEditor::inputUpdate()
	{
		ValueEditor::valueEditorUpdate();
		GUIRectangle::inputUpdate();

		//Disable input receive?
		if (disableInputReceiveOnNextUpdate)
		{
			stringUpdated = false;
			disableState(GUIRECT_RECEIVING_INPUT_BIT);
			disableInputReceiveOnNextUpdate = false;
		}

		if (getInputEnabled())
		{//When enabled

			//Receiving input
			if (isReceivingInput())
			{
				recordInput(getDeltaTimeSystem().deltaTime);
				typeCharacter->setRenderState(getRenderState() && ((typerBlinkTimer.value % typerBlinkTime.value) < typerBlinkTime.value / 2));
				typerBlinkTimer += getDeltaTimeSystem().deltaTime;
			}
			else
			{
				typeCharacter->setRenderState(false);
				keyboardRecorder.stop();
			}

			if (!editorValueChanged() && getMouseHover() && getInputManager().isKeyPressed(MOUSE_BUTTON_LEFT))
			{//Mouse press
				toggleTyping();
			}
		}
		else
		{
			typeCharacter->setRenderState(false);
			keyboardRecorder.stop();
		}

		if (!stringUpdated)
			updateString();
	}

	void GUIStringEditor::updateString()
	{
		if (isReceivingInput())
		{//String while typing
			text->setString(input);
		}
		else//If not typing, use default string
		{
			if (!text)
			{
				text = getBatchManager().createText(getDepth() + 2);
				text->setFont("Fonts/Anonymous.ttf", 12);
			}
			text->setString(defaultString);
		}
		stringUpdated = true;

		disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED_BIT);
		disableStateRecursiveUpwards(GUIRECT_POSITION_UPDATED_BIT);
		disableStateRecursiveUpwards(GUIRECT_SCALE_UPDATED_BIT);
	}

	void GUIStringEditor::setString(const std::string& str)
	{
		GUIRectangle::setString(str);
		defaultString = str;
		stringUpdated = false;
	}

	void GUIStringEditor::toggleTyping()
	{
		if (isReceivingInput())
			endTyping();
		else
			beginTyping();
	}

	void GUIStringEditor::beginTyping()
	{
		if (isReceivingInput())
			return;
		enableBit(state, GUIRECT_RECEIVING_INPUT_BIT);
		typerPosition = int(getEditorValue().size());
		typerBlinkTimer = 0;
		stringUpdated = false;
		input = getEditorValue();
	}

	void GUIStringEditor::endTyping()
	{
		if (!isReceivingInput())
			return;
		//NOTE: Do not disable input receive here! Must remain receiving until end of update loop!
		disableInputReceiveOnNextUpdate = true;
		typerPosition = 0;
		typerBlinkTimer = 0;
		setEditorValue(input);
	}

	void GUIStringEditor::recordInput(const time::Time deltaTime)
	{
		keyboardRecorder.update(deltaTime);

		//CHARACTER INPUT
		if (input.size() < maxStringLength)
		{
			for (unsigned i = 0; i < keyboardRecorder.characterInput.size(); i++)
			{
				input.insert(input.begin() + typerPosition, keyboardRecorder.characterInput[i]);
				stringUpdated = false;
				typerPosition++;
				typerBlinkTimer = 0;
			}
		}

		//COMMAND INPUT
		const bool ctrl(getInputManager().isKeyDown(KEYBOARD_LCTRL) || getInputManager().isKeyDown(KEYBOARD_RCTRL));
		const bool shift(getInputManager().isKeyDown(KEYBOARD_LSHIFT) || getInputManager().isKeyDown(KEYBOARD_RSHIFT));
		for (size_t c = 0; c < keyboardRecorder.commandInput.size(); c++)
		{
			switch (keyboardRecorder.commandInput[c])
			{
			case KEYBOARD_BACKSPACE:
				if (typerPosition > 0)
				{
					int length(1);
					if (ctrl)
					{
						length = 0;
						for (int j = typerPosition - 1; j >= 0; j--)
						{
							if (input[size_t(j)] == ' ')
								break;
							length++;
						}
						if (length == 0)
							length = 1;//Erase the space
					}

					typerPosition -= length;
					typerBlinkTimer = 0;

					input.erase(input.begin() + typerPosition, input.begin() + typerPosition + length);
					stringUpdated = false;
				}
				break;
			case KEYBOARD_DELETE:
				if (size_t(typerPosition) < input.size())
				{
					int length(1);
					if (ctrl)
					{//Erase to the end of current word
						length = 0;
						for (size_t j = typerPosition; j < input.size(); j++)
						{
							if (input[j] == ' ')
								break;
							length++;
						}
						if (length == 0)
							length = 1;//Erase the space
					}
					input.erase(input.begin() + typerPosition, input.begin() + typerPosition + length);
					stringUpdated = false;
				}
				break;
			case KEYBOARD_LEFT:
			{
				int length = 1;
				if (ctrl)
				{
					length = 0;
					for (int j = typerPosition - 1; j >= 0; j--)
					{
						if (input[j] == ' ')
							break;
						length++;
					}
					if (length == 0)
						length = 1;//Skip the space
				}
				typerPosition = std::max(0, typerPosition - length);
				typerBlinkTimer = 0;
				stringUpdated = false;
			}
			break;
			case KEYBOARD_RIGHT:
			{
				int length = 1;
				if (ctrl)
				{
					length = 0;
					for (size_t j = size_t(typerPosition); j < input.size(); j++)
					{
						if (input[j] == ' ')
							break;
						length++;
					}
					if (length == 0)
						length = 1;
				}
				typerPosition = std::min((int)input.size(), typerPosition + length);
				typerBlinkTimer = 0;
				stringUpdated = false;
			}
			break;
			case KEYBOARD_END:
				typerPosition = int(input.size());
				typerBlinkTimer = 0;
				stringUpdated = false;
				break;
			case KEYBOARD_HOME:
				typerPosition = 0;
				typerBlinkTimer = 0;
				stringUpdated = false;
				break;
			case KEYBOARD_RETURN:
			case KEYBOARD_KP_ENTER:
				if (multilineEditing)
				{
					input.insert(input.begin() + typerPosition, '\n');
					stringUpdated = false;
					typerPosition++;
					typerBlinkTimer = 0;
				}
				else
					endTyping();
				break;
			case KEYBOARD_ESCAPE:
				endTyping();
				break;
			}
		}

		//End typing
		if (getInputManager().isKeyPressed(MOUSEBUTTON_LEFT))
			endTyping();
	}

	float GUIStringEditor::getEditorValueAsFloat() const
	{
		return getStringAsFloat(getEditorValue());
	}

	int GUIStringEditor::getEditorValueAsInt() const
	{
		return getStringAsInt(getEditorValue());
	}

	void GUIStringEditor::onDisableInput()
	{
		endTyping();
		stringUpdated = false;
		GUIRectangle::onDisableInput();
	}

	void GUIStringEditor::onEditorValueChange()
	{
		stringUpdated = false;
	}
}