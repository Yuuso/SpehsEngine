#include "ApplicationData.h"
#include "Text.h"
#include "InputManager.h"
#include "StringOperations.h"
#include "GUITextField.h"


namespace spehs
{
	GUITextField::GUITextField() : stringUpdated(false), defaultString(""), input("")
	{
	}
	GUITextField::GUITextField(GUIRECT_ID_TYPE ID) : GUIRectangle(ID)
	{
	}
	GUITextField::GUITextField(int w, int h) : GUIRectangle(w, h)
	{
	}
	GUITextField::~GUITextField()
	{
	}
	void GUITextField::update()
	{
		GUIRectangle::update();

		//Receiving input
		if (isReceivingInput())
			recordInput();

		if (getMouseHover())
		{
			//Toggle typing if mouse press is detected
			if (inputManager->isKeyPressed(MOUSE_BUTTON_LEFT))
				toggleTyping();
		}

		if (!stringUpdated)
			updateString();

	}
	void GUITextField::setString(std::string str)
	{
		GUIRectangle::setString(str);
		defaultString = str;
		stringUpdated = false;
	}
	void GUITextField::updateString()
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
				text = new spehs::Text(getDepth() + 2);
				text->setFont(applicationData->GUITextFontPath, applicationData->GUITextSize);
			}
			text->setString(defaultString);
		}
		stringUpdated = true;

		disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED);
		disableStateRecursiveUpwards(GUIRECT_POSITIONED);
		disableStateRecursiveUpwards(GUIRECT_SCALED);
	}
	void GUITextField::toggleTyping()
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
	void GUITextField::beginTyping()
	{
		if (isReceivingInput())
			return;
		enableBit(state, GUIRECT_RECEIVING_INPUT);
		getFirstGenerationParent()->enableState(GUIRECT_RECEIVING_INPUT);
		stringUpdated = false;
	}
	void GUITextField::endTyping()
	{
		if (!isReceivingInput())
			return;
		disableBit(state, GUIRECT_RECEIVING_INPUT);
		getFirstGenerationParent()->disableState(GUIRECT_RECEIVING_INPUT);
		input = "";//Reset input string
		stringUpdated = false;
	}
	void GUITextField::recordInput()
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
		if (inputManager->isKeyPressed(KEYBOARD_RETURN))
		{
			storedString = input;//Store input string
			endTyping();
		}
		if (inputManager->isKeyPressed(KEYBOARD_KP_ENTER))
		{
			storedString = input;//Store input string
			endTyping();
		}
		if (inputManager->isKeyPressed(KEYBOARD_ESCAPE))
		{
			endTyping();
		}

	}

	bool GUITextField::stringReady()
	{
		if (storedString.size() > 0)
			return true;
		return false;
	}
	std::string GUITextField::retrieveString()
	{
		std::string returnValue = storedString;
		storedString.clear();
		return returnValue;
	}
	float GUITextField::retrieveStringAsFloat()
	{
		return getStringAsFloat(storedString);
	}
	int GUITextField::retrieveStringAsInt()
	{
		return getStringAsInt(storedString);
	}
	void GUITextField::loseFocus()
	{
		endTyping();
		updateString();
		GUIRectangle::loseFocus();
	}
}