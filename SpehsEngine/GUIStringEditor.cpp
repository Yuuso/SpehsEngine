#include "ApplicationData.h"
#include "Text.h"
#include "InputManager.h"
#include "StringOperations.h"
#include "GUIStringEditor.h"


namespace spehs
{
	GUIStringEditor::GUIStringEditor() : stringUpdated(false), defaultString(""), input(""), storedString(""), disableInputReceiveOnNextUpdate(false)
	{
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
	void GUIStringEditor::inputUpdate()
	{
		GUIRectangle::inputUpdate();

		//Disable input receive?
		if (disableInputReceiveOnNextUpdate)
		{
			stringUpdated = false;
			disableState(GUIRECT_RECEIVING_INPUT);
			disableInputReceiveOnNextUpdate = false;
		}

		//Clear edited state
		storedString.clear();

		if (checkState(GUIRECT_INPUT_ENABLED_BIT))
		{//When enabled

			//Receiving input
			if (isReceivingInput())
				recordInput();

			if (getMouseHover() && inputManager->isKeyPressed(MOUSE_BUTTON_LEFT))
			{//Mouse press
				toggleTyping();
			}
		}

		if (!stringUpdated)
			updateString();

	}
	void GUIStringEditor::updateString()
	{
		if (isReceivingInput())
		{//String while typing
			if (input.size() == 0)
				text->setString("<Enter text>");
			else
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
		GUIRectangle::setString(str);
		defaultString = str;
		stringUpdated = false;
	}
	void GUIStringEditor::toggleTyping()
	{
		if (isReceivingInput())
		{
			//Store input and end typing process
			storedString = input;
			endTyping();
		}
		else
			beginTyping();
	}
	void GUIStringEditor::beginTyping()
	{
		if (isReceivingInput())
			return;
		enableBit(state, GUIRECT_RECEIVING_INPUT);
		stringUpdated = false;
	}
	void GUIStringEditor::endTyping()
	{
		if (!isReceivingInput())
			return;
		//NOTE: Do not disable input receive here! Must remain receiving until end of update loop!
		disableInputReceiveOnNextUpdate = true;
		input = "";//Reset input string
	}
	void GUIStringEditor::recordInput()
	{
		//Alphabet
		int capital = 0;
		if (inputManager->isKeyDown(KEYBOARD_LSHIFT) || inputManager->isKeyDown(KEYBOARD_RSHIFT))
			capital = -32;
		for (unsigned i = 97; i <= 122; i++)
		{
			if (inputManager->isKeyPressed(i))
			{
				input += char(i + capital);
				stringUpdated = false;
			}
		}

		//Numbers
		for (int i = 48; i <= 57; i++)
			if (inputManager->isKeyPressed(i))
			{
				input += char(i);
				stringUpdated = false;
			}
		for (int i = KEYBOARD_KP_1; i <= KEYBOARD_KP_9; i++)
			if (inputManager->isKeyPressed(i))
			{
				input += char(i - KEYBOARD_KP_1 + 49);
				stringUpdated = false;
			}
		if (inputManager->isKeyPressed(KEYBOARD_KP_0))
		{
			input += '0';
			stringUpdated = false;
		}

		//Special characters
		if (inputManager->isKeyPressed(KEYBOARD_SPACE))
		{
			input += ' ';
			stringUpdated = false;
		}
		if (inputManager->isKeyPressed(KEYBOARD_PERIOD))
		{
			input += '.';
			stringUpdated = false;
		}
		if (inputManager->isKeyPressed(KEYBOARD_COMMA))
		{
			input += ',';
			stringUpdated = false;
		}
		if (inputManager->isKeyPressed(KEYBOARD_KP_PERIOD))
		{
			input += ',';
			stringUpdated = false;
		}
		if (inputManager->isKeyPressed(KEYBOARD_MINUS))
		{
			input += '-';
			stringUpdated = false;
		}
		if (inputManager->isKeyPressed(KEYBOARD_KP_MINUS))
		{
			input += '-';
			stringUpdated = false;
		}

		//Backspace/character deletion
		if (inputManager->isKeyPressed(KEYBOARD_BACKSPACE) && input.size() > 0)
		{
			input.erase(input.begin() + input.size() - 1);
			stringUpdated = false;
		}

		//End typing
		if (inputManager->isKeyPressed(KEYBOARD_RETURN) ||
			inputManager->isKeyPressed(KEYBOARD_KP_ENTER) ||
			(inputManager->isKeyPressed(MOUSEBUTTON_LEFT) && getMouseHover()))
		{
			storedString = input;//Store input string
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

	bool GUIStringEditor::valueEdited() const
	{
		return storedString.size() > 0;
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