#pragma once
#include "KeyboardRecorder.h"
#include "GUIRectangle.h"
#include "ValueEditor.h"
#include <string>
#include <vector>

namespace spehs
{
	class GUIStringEditor : public GUIRectangle, public ValueEditor<std::string>
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
		float getEditorValueAsFloat() const;
		int getEditorValueAsInt() const;
		void setMaxStringLength(const int length) { maxStringLength = length; }
		void setTyperBlinkTime(const int milliSeconds) { typerBlinkTime = 2 * milliSeconds; }

		//Identity
		GUIStringEditor* getAsGUIStringEditorPtr() override { return this; }

	protected:
		void onEditorValueChange() override;

		//Text field specific
		void toggleTyping();
		void beginTyping();
		void recordInput();
		void updateString();
		void endTyping();///< Ends input receiving, storing any input that had been received until now

		bool stringUpdated;
		bool disableInputReceiveOnNextUpdate;
		int maxStringLength;
		int typerPosition;
		int typerBlinkTime;
		int typerBlinkTimer;
		std::string defaultString;
		std::string input;//Temporal storage for edited value so that value won't appear changed after each individual typed character
		spehs::Text* typeCharacter;
		KeyboardRecorder keyboardRecorder;
	};
}