#include <algorithm>
#include "StringOperations.h"
#include "GUIStringEditor.h"
#include "ApplicationData.h"
#include "InputManager.h"
#include "Text.h"
#include "Time.h"


namespace spehs
{
	int GUIStringEditor::defaultMaxStringEditorStringLength = 32;
	GUIStringEditor::GUIStringEditor() : stringEdited(false), stringUpdated(false), defaultString(""), input(""), storedString(""),
		disableInputReceiveOnNextUpdate(false), maxStringLength(defaultMaxStringEditorStringLength),
		backspaceTimer(0.0f), firstBackspace(false), keyRepeatTimer(0.0f), lastInputChar(0), typerPosition(0)
	{
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
				typeCharacter->setRenderState(getRenderState() && ((spehs::time::getRunTime().asMilliseconds % 1024) >= 512));
			}

			if (!stringEdited && getMouseHover() && inputManager->isKeyPressed(MOUSE_BUTTON_LEFT))
			{//Mouse press
				toggleTyping();
			}

		}
		else
			typeCharacter->setRenderState(false);

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
	}
	void GUIStringEditor::recordInput()
	{
		if (input.size() < maxStringLength)
		{//Increment charater

			char inputChar(0);

			//Alphabet
			int capital = 0;
			if (inputManager->isKeyDown(KEYBOARD_LSHIFT) || inputManager->isKeyDown(KEYBOARD_RSHIFT))
				capital = -32;
			for (unsigned i = 97; i <= 122; i++)
			{
				if (inputManager->isKeyDown(i))
				{
					inputChar = char(i + capital);
					if (inputManager->isKeyPressed(i))
						keyRepeatTimer = 0.0f;
				}
			}

			//Numbers
			for (int i = 48; i <= 57; i++)
			{
				if (inputManager->isKeyDown(i))
				{
					inputChar = char(i);
					if (inputManager->isKeyPressed(i))
						keyRepeatTimer = 0.0f;
				}
			}
			for (int i = KEYBOARD_KP_1; i <= KEYBOARD_KP_9; i++)
			{
				if (inputManager->isKeyDown(i))
				{
					inputChar = char(i - KEYBOARD_KP_1 + 49);
					if (inputManager->isKeyPressed(i))
						keyRepeatTimer = 0.0f;
				}
			}
			if (inputManager->isKeyDown(KEYBOARD_KP_0))
			{
				inputChar = '0';
				if (inputManager->isKeyPressed(KEYBOARD_KP_0))
					keyRepeatTimer = 0.0f;
			}

			//Special characters
			if (inputManager->isKeyDown(KEYBOARD_SPACE))
			{
				inputChar = ' ';
				if (inputManager->isKeyPressed(KEYBOARD_SPACE))
					keyRepeatTimer = 0.0f;
			}
			if (inputManager->isKeyDown(KEYBOARD_COMMA))
			{
				inputChar = ',';
				if (inputManager->isKeyPressed(KEYBOARD_COMMA))
					keyRepeatTimer = 0.0f;
			}
			if (inputManager->isKeyDown(KEYBOARD_KP_PERIOD) || inputManager->isKeyDown(KEYBOARD_PERIOD))
			{
				inputChar = '.';
				if (inputManager->isKeyPressed(KEYBOARD_KP_PERIOD) || inputManager->isKeyPressed(KEYBOARD_PERIOD))
					keyRepeatTimer = 0.0f;
			}
			if (inputManager->isKeyDown(KEYBOARD_KP_MINUS) || inputManager->isKeyDown(KEYBOARD_MINUS))
			{
				inputChar = '-';
				if (inputManager->isKeyPressed(KEYBOARD_KP_MINUS) || inputManager->isKeyPressed(KEYBOARD_MINUS))
					keyRepeatTimer = 0.0f;
			}

			if (inputChar)
			{//Input detected

				if (inputChar == lastInputChar)
				{
					keyRepeatTimer -= spehs::time::getDeltaTimeAsSeconds();
					if (keyRepeatTimer <= 0.0f)
					{
						input.insert(input.begin() + typerPosition, inputChar);
						stringUpdated = false;
						keyRepeatTimer = 0.5f;
						typerPosition++;
					}
				}
				else
				{//Different input char
					input.insert(input.begin() + typerPosition, inputChar);
					stringUpdated = false;
					keyRepeatTimer = 0.5f;
					typerPosition++;
				}

				lastInputChar = inputChar;
			}
		}

		//Backspace/character deletion
		if (inputManager->isKeyDown(KEYBOARD_BACKSPACE) && typerPosition > 0)
		{
			if (backspaceTimer <= 0.0f)
			{
				input.erase(input.begin() + --typerPosition);
				stringUpdated = false;
				if (firstBackspace)
					backspaceTimer = 1.0f;
				else
					backspaceTimer = 0.05f;
				firstBackspace = false;
			}
			else
				backspaceTimer -= spehs::time::getDeltaTimeAsSeconds();
		}
		else
		{
			backspaceTimer = 0.0f;
			firstBackspace = true;
		}
		if (inputManager->isKeyPressed(KEYBOARD_DELETE) && typerPosition <= input.size())
		{
			input.erase(input.begin() + typerPosition);
			stringUpdated = false;
		}

		//Moving the typer
		if (inputManager->isKeyPressed(KEYBOARD_LEFT))
		{
			typerPosition = std::max(0, typerPosition - 1);
			stringUpdated = false;
		}
		if (inputManager->isKeyPressed(KEYBOARD_RIGHT))
		{
			typerPosition = std::min((int)input.size(), typerPosition + 1);
			stringUpdated = false;
		}

		//End typing
		if (inputManager->isKeyPressed(KEYBOARD_RETURN) ||
			inputManager->isKeyPressed(KEYBOARD_KP_ENTER) ||
			(inputManager->isKeyPressed(MOUSEBUTTON_LEFT) && getMouseHover()))
		{
			endTyping();

			//Make press callback call
			if (pressCallbackFunction)
				(*pressCallbackFunction)(*this);
		}
		if (inputManager->isKeyPressed(KEYBOARD_ESCAPE) ||
			inputManager->isKeyPressed(MOUSEBUTTON_LEFT) && !getMouseHover())
		{
			endTyping();
		}

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