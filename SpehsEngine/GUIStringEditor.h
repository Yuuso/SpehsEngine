#pragma once
#include "GUIEditor.h"
#include <string>

namespace spehs
{
	class GUIStringEditor : public GUIEditor
	{
	public:
		GUIStringEditor();
		GUIStringEditor(std::string str);
		GUIStringEditor(GUIRECT_ID_TYPE id);
		GUIStringEditor(int, int);
		~GUIStringEditor();

		void inputUpdate();
		void setString(std::string str);
		void onDisableInput();


		//Editor
		bool valueEdited();
		std::string retrieveString();
		float retrieveStringAsFloat();
		int retrieveStringAsInt();

		//Identity
		GUIStringEditor* getAsGUIStringEditorPtr(){ return this; }

	protected:
		//Text field specific
		void toggleTyping();
		void beginTyping();
		void recordInput();
		void updateString();
		void endTyping();

		bool stringUpdated;
		bool disableInputReceiveOnNextUpdate;
		std::string defaultString;
		std::string input;
		std::string storedString;
	};
}