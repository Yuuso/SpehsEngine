#pragma once
#include "GUIEditor.h"
#include <string>

namespace spehs
{
	class GUIStringEditor : public GUIEditor
	{
	public:
		GUIStringEditor();
		GUIStringEditor(const std::string str);
		GUIStringEditor(const GUIRECT_ID_TYPE id);
		GUIStringEditor(const int width, const int height);
		~GUIStringEditor() override;

		void inputUpdate() override;
		void setString(const std::string str) override;
		void onDisableInput() override;


		//Editor
		bool valueEdited() const override;
		std::string retrieveString() const;
		float retrieveStringAsFloat() const;
		int retrieveStringAsInt() const;

		//Identity
		GUIStringEditor* getAsGUIStringEditorPtr() override { return this; }

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