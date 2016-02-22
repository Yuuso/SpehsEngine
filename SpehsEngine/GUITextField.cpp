#include "ApplicationData.h"
#include "Text.h"
#include "InputManager.h"
#include "GUITextField.h"


namespace SpehsEngine
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
				text = new SpehsEngine::Text();
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
		if (inputManager->isKeyPressed(KEYBOARD_MINUS))
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
		int8_t stringState = 0;
		int intValue = 0;
		float floatValue = 0.0f;
		for (unsigned i = 0; i < storedString.size(); i++)
		{
			if (storedString[i] >= '0' && storedString[i] <= '9')
			{//Add numerical value
				if (checkBit(stringState, 2))
				{//Decimal
					floatValue *= 0.1f;
					floatValue += int(storedString[i] - 48) * 0.1f;
				}
				else
				{
					intValue *= 10;
					intValue += int(storedString[i] - 48);
				}
			}
			else if (storedString[i] == '-')
			{
				if (checkBit(stringState, 1))
				{
					storedString.clear();
					return 0.0f;
				}
				enableBit(stringState, 1);//Negative
			}
			else if (storedString[i] == '.' || storedString[i] == ',')
			{
				if (checkBit(stringState, 2))
				{
					storedString.clear();
					return 0.0f;
				}
				enableBit(stringState, 2);//Begin decimal part
			}
			else
			{//Character is unknown
				storedString.clear();
				return 0.0f;
			}
		}
		storedString.clear();

		//Add up values, negate if needed
		floatValue += intValue;
		if (checkBit(stringState, 1))
			floatValue *= -1;

		return floatValue;
	}
	void GUITextField::loseFocus()
	{
		endTyping();
		updateString();
		GUIRectangle::loseFocus();
	}
}