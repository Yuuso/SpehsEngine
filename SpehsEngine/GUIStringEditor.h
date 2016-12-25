#pragma once
#include "KeyboardRecorder.h"
#include "GUIEditor.h"
#include <string>
#include <vector>

namespace spehs
{
	class GUIStringEditor : public GUIEditor
	{
		static int defaultMaxStringEditorStringLength;
	public:
		GUIStringEditor();
		GUIStringEditor(const std::string str);
		GUIStringEditor(const GUIRECT_ID_TYPE id);
		GUIStringEditor(const int width, const int height);
		~GUIStringEditor() override;

		void setRenderState(const bool _state) override;
		void setDepth(const int16_t depth) override;
		void setString(const std::string str) override;
		void setStringSize(const int size) override;
		void setStringSizeRelative(const int relativeSize) override;
		void setStringColor(const glm::vec4& col) override;
		void setStringColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a = 255) override;
		void setStringAlpha(const float alpha) override;
		void setStringAlpha(const unsigned char a) override;
		void updatePosition() override;
		void updateMinSize() override;
		void inputUpdate() override;
		void onDisableInput() override;


		//Editor
		bool valueEdited() const override { return stringEdited; }
		std::string retrieveString() const;
		float retrieveStringAsFloat() const;
		int retrieveStringAsInt() const;
		void setMaxStringLength(const int length) { maxStringLength = length; }
		void setTyperBlinkTime(const int milliSeconds) { typerBlinkTime = 2 * milliSeconds; }

		//Identity
		GUIStringEditor* getAsGUIStringEditorPtr() override { return this; }

	protected:
		//Text field specific
		void toggleTyping();
		void beginTyping();
		void recordInput();
		void updateString();
		void endTyping();///< Ends input receiving, storing any input that had been received until now

		bool stringEdited;
		bool stringUpdated;
		bool disableInputReceiveOnNextUpdate;
		int maxStringLength;
		int typerPosition;
		int typerBlinkTime;
		int typerBlinkTimer;
		std::string defaultString;
		std::string input;
		std::string storedString;
		spehs::Text* typeCharacter;
		KeyboardRecorder keyboardRecorder;
	};
}