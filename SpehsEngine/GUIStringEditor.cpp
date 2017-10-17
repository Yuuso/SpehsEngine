#include <algorithm>
#include "StringOperations.h"
#include "GUIStringEditor.h"
#include "ApplicationData.h"
#include "BatchManager.h"
#include "InputManager.h"
#include "Text.h"
#include "Time.h"
#include "OS.h"


namespace spehs
{
	int GUIStringEditor::defaultMaxStringEditorStringLength = 32;

	GUIStringEditor::GUIStringEditor(BatchManager& _batchManager)
		: GUIRectangle(_batchManager)
		, ValueEditor("")
		, stringUpdated(false)
		, defaultString("")
		, input("")
		, disableInputReceiveOnNextUpdate(false)
		, maxStringLength(defaultMaxStringEditorStringLength)
		, typerPosition(0)
		, typerBlinkTime(0)
		, typerBlinkTimer(0)
		, multilineEditing(false)
	{
		setTyperBlinkTime(time::seconds(0.5f));
		createText();
		typeCharacter = batchManager.createText("|", getDepth() + 1);
		typeCharacter->setRenderState(getRenderState() && isReceivingInput());
		typeCharacter->setFont(spehs::ApplicationData::GUITextFontPath, spehs::ApplicationData::GUITextSize);
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

	void GUIStringEditor::setStringSize(const int size)
	{
		GUIRectangle::setStringSize(size);
		typeCharacter->setFontSize(size);
	}

	void GUIStringEditor::setStringSizeRelative(const int relativeSize)
	{
		GUIRectangle::setStringSizeRelative(relativeSize);
		typeCharacter->setFontSize(spehs::ApplicationData::GUITextSize + relativeSize);
	}

	void GUIStringEditor::setStringColor(const Color& col)
	{
		GUIRectangle::setStringColor(col);
		typeCharacter->setColor(col);
	}

	void GUIStringEditor::setStringAlpha(const Color::Component& a)
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
		minSize.x += typeCharacter->getTextWidth();
	}

	void GUIStringEditor::inputUpdate(InputUpdateData& data)
	{
		ValueEditor::update();
		GUIRectangle::inputUpdate(data);

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
				recordInput(data.deltaTime);
				typeCharacter->setRenderState(getRenderState() && ((typerBlinkTimer.value % typerBlinkTime.value) < typerBlinkTime.value / 2));
				typerBlinkTimer += data.deltaTime;
			}
			else
			{
				typeCharacter->setRenderState(false);
				keyboardRecorder.stop();
			}

			if (!editorValueChanged() && getMouseHover() && inputManager->isKeyPressed(MOUSE_BUTTON_LEFT))
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
				text = batchManager.createText(getDepth() + 2);
				text->setFont(spehs::ApplicationData::GUITextFontPath, spehs::ApplicationData::GUITextSize);
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
		typerPosition = editorValue.size();
		typerBlinkTimer = 0;
		stringUpdated = false;
		input = editorValue;
	}

	void GUIStringEditor::endTyping()
	{
		if (!isReceivingInput())
			return;
		//NOTE: Do not disable input receive here! Must remain receiving until end of update loop!
		disableInputReceiveOnNextUpdate = true;
		typerPosition = 0;
		typerBlinkTimer = 0;
		editorValue = input;
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
		const bool ctrl(inputManager->isKeyDown(KEYBOARD_LCTRL) || inputManager->isKeyDown(KEYBOARD_RCTRL));
		const bool shift(inputManager->isKeyDown(KEYBOARD_LSHIFT) || inputManager->isKeyDown(KEYBOARD_RSHIFT));
		for (unsigned i = 0; i < keyboardRecorder.commandInput.size(); i++)
		{
			switch (keyboardRecorder.commandInput[i])
			{
			case KEYBOARD_BACKSPACE:
				if (typerPosition > 0)
				{
					int length(1);
					if (ctrl)
					{
						length = 0;
						for (int i = typerPosition - 1; i >= 0; i--)
						{
							if (input[i] == ' ')
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
				if (typerPosition < input.size())
				{
					int length(1);
					if (ctrl)
					{//Erase to the end of current word
						length = 0;
						for (unsigned i = typerPosition; i < input.size(); i++)
						{
							if (input[i] == ' ')
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
					for (int i = typerPosition - 1; i >= 0; i--)
					{
						if (input[i] == ' ')
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
					for (int i = typerPosition; i < input.size(); i++)
					{
						if (input[i] == ' ')
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
				typerPosition = input.size();
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
		if (inputManager->isKeyPressed(MOUSEBUTTON_LEFT))
			endTyping();
	}

	float GUIStringEditor::getEditorValueAsFloat() const
	{
		return getStringAsFloat(editorValue);
	}

	int GUIStringEditor::getEditorValueAsInt() const
	{
		return getStringAsInt(editorValue);
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