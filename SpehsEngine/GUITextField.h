#pragma once
#include "GUIRectangle.h"
#include <string>

namespace SpehsEngine
{
	class GUITextField : public GUIRectangle
	{
	public:
		GUITextField();
		GUITextField(GUIRECT_ID_TYPE id);
		GUITextField(int, int);
		~GUITextField();

		void update();
		void setString(std::string str);

		void loseFocus();
		void toggleTyping();
		void beginTyping();
		void recordInput();
		void updateString();
		void endTyping();
		/*Returns true if string has been entered and is ready to be retrieved*/
		bool stringReady();
		/*Returns the stored string. [!]Resets string storeage to accept next string element*/
		std::string retrieveString();
		float retrieveStringAsFloat();
		int retrieveStringAsInt();

		//Identity
		GUITextField* getAsGUITextFieldPtr(){ return this; }

	protected:

		bool stringUpdated;
		std::string defaultString;
		std::string input;
		std::string storedString;
	};
}