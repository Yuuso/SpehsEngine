#include <algorithm>
#include "StringOperations.h"
#include "GUIStringEditor.h"
#include "ApplicationData.h"
#include "InputManager.h"
#include "Text.h"
#include "Time.h"
#include "OS.h"


namespace spehs
{
	int GUIStringEditor::defaultMaxStringEditorStringLength = 32;
	GUIStringEditor::GUIStringEditor() : stringEdited(false), stringUpdated(false), defaultString(""), input(""), storedString(""),
		disableInputReceiveOnNextUpdate(false), maxStringLength(defaultMaxStringEditorStringLength), typerPosition(0), typerBlinkTime(0), typerBlinkTimer(0)
	{
		setTyperBlinkTime(512);
		createText();
		typeCharacter = spehs::Text::create("|", getDepth() + 1);
		typeCharacter->setRenderState(getRenderState() && isReceivingInput());
		typeCharacter->setFont(applicationData->GUITextFontPath, applicationData->GUITextSize);
		typeCharacter->setColor(text->getColor());
	}
	GUIStringEditor::GUIStringEditor(const std::string str) : GUIStringEditor()
	{
		setString(str);
	}
	GUIStringEditor::GUIStringEditor(const GUIRECT_ID_TYPE ID) : GUIStringEditor()
	{
		setID(ID);
	}
	GUIStringEditor::GUIStringEditor(const int w, const int h) : GUIStringEditor()
	{
		setSize(w, h);
	}
	GUIStringEditor::~GUIStringEditor()
	{
	}
	void GUIStringEditor::setRenderState(const bool _state)
	{
		GUIEditor::setRenderState(_state);
		typeCharacter->setRenderState(_state && isReceivingInput());
	}
	void GUIStringEditor::setDepth(const int16_t depth)
	{
		GUIEditor::setDepth(depth);
		typeCharacter->setPlaneDepth(depth + 1);
	}	
	void GUIStringEditor::setStringSize(const int size)
	{
		GUIEditor::setStringSize(size);
		typeCharacter->setFontSize(size);
	}
	void GUIStringEditor::setStringSizeRelative(const int relativeSize)
	{
		GUIEditor::setStringSizeRelative(relativeSize);
		typeCharacter->setFontSize(applicationData->GUITextSize + relativeSize);
	}
	void GUIStringEditor::setStringColor(const glm::vec4& col)
	{
		GUIEditor::setStringColor(col);
		typeCharacter->setColor(col);
	}
	void GUIStringEditor::setStringColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
	{
		GUIEditor::setStringColor(r, g, b, a);
		typeCharacter->setColor((unsigned char)r, g, b, a);
	}
	void GUIStringEditor::setStringAlpha(const float alpha)
	{
		GUIEditor::setStringAlpha(alpha);
		typeCharacter->setAlpha(alpha);
	}
	void GUIStringEditor::setStringAlpha(const unsigned char a)
	{
		GUIEditor::setStringAlpha(a);
		typeCharacter->setAlpha(a);
	}
	void GUIStringEditor::updatePosition()
	{
		GUIEditor::updatePosition();
		typeCharacter->setPosition(text->getX(typerPosition) - typeCharacter->getTextWidth() * 0.5f, text->getY());
	}
	void GUIStringEditor::updateMinSize()
	{
		GUIEditor::updateMinSize();
		minSize += typeCharacter->getTextWidth();
	}
	void GUIStringEditor::inputUpdate()
	{
		stringEdited = false;
		GUIRectangle::inputUpdate();

		//Disable input receive?
		if (disableInputReceiveOnNextUpdate)
		{
			stringUpdated = false;
			disableState(GUIRECT_RECEIVING_INPUT);
			disableInputReceiveOnNextUpdate = false;
		}

		if (getInputEnabled())
		{//When enabled

			//Receiving input
			if (isReceivingInput())
			{
				recordInput();
				typeCharacter->setRenderState(getRenderState() && ((typerBlinkTimer % typerBlinkTime) < typerBlinkTime / 2));
				typerBlinkTimer += time::getDeltaTimeAsMilliseconds();
			}
			else
				keyboardRecorder.stop();

			if (!stringEdited && getMouseHover() && inputManager->isKeyPressed(MOUSE_BUTTON_LEFT))
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
				text = Text::create(getDepth() + 2);
				text->setFont(applicationData->GUITextFontPath, applicationData->GUITextSize);
			}
			text->setString(defaultString);
		}
		stringUpdated = true;

		disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED_BIT);
		disableStateRecursiveUpwards(GUIRECT_POSITION_UPDATED_BIT);
		disableStateRecursiveUpwards(GUIRECT_SCALE_UPDATED_BIT);
	}
	void GUIStringEditor::setString(const std::string str)
	{
		GUIEditor::setString(str);
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
		enableBit(state, GUIRECT_RECEIVING_INPUT);
		input = storedString;
		typerPosition = input.size();
		typerBlinkTimer = 0;
		stringUpdated = false;
	}
	void GUIStringEditor::endTyping()
	{
		if (!isReceivingInput())
			return;
		//NOTE: Do not disable input receive here! Must remain receiving until end of update loop!
		disableInputReceiveOnNextUpdate = true;
		storedString = input;//Store input string
		stringEdited = true;
		input = "";//Reset input string
		typerPosition = 0;
		typerBlinkTimer = 0;
	}
	void GUIStringEditor::recordInput()
	{
		keyboardRecorder.update();

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
				endTyping();
				break;
			}
		}

		//End typing
		if (inputManager->isKeyPressed(MOUSEBUTTON_LEFT))
			endTyping();
	}

	std::string GUIStringEditor::retrieveString() const
	{
		return storedString;
	}
	float GUIStringEditor::retrieveStringAsFloat() const
	{
		return getStringAsFloat(storedString);
	}
	int GUIStringEditor::retrieveStringAsInt() const
	{
		return getStringAsInt(storedString);
	}
	void GUIStringEditor::onDisableInput()
	{
		endTyping();
		stringUpdated = false;
		GUIRectangle::onDisableInput();
	}
}