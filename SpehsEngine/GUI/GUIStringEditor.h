#pragma once
#include "SpehsEngine/Input/KeyboardRecorder.h"
#include "SpehsEngine/GUI/GUIRectangle.h"
#include "SpehsEngine/GUI/ValueEditor.h"
#include <string>
#include <vector>

namespace se
{
	class GUIStringEditor : public GUIRectangle, public ValueEditor<std::string>
	{
		static size_t defaultMaxStringEditorStringLength;
	public:
		GUIStringEditor(GUIContext& context, const std::string& initialValue);
		~GUIStringEditor() override;

		void setRenderState(const bool _state) override;
		void setDepth(const int16_t depth) override;
		void setString(const std::string& str) override;
		void setStringSize(const int size) override;
		void setStringSizeRelative(const int relativeSize) override;
		void setStringColor(const se::Color& col) override;
		void setStringAlpha(const float a) override;
		void updatePosition() override;
		void updateMinSize() override;
		void inputUpdate() override;
		void onDisableInput() override;

		//Editor
		float getEditorValueAsFloat() const;
		int getEditorValueAsInt() const;
		void setMaxStringLength(const int length) { maxStringLength = length; }
		void setTyperBlinkTime(const time::Time time) { typerBlinkTime = time * 2; }
		void setMultiLineEditing(const bool _state) { multilineEditing = _state; }

		//Identity
		GUIStringEditor* getAsGUIStringEditorPtr() override { return this; }

	protected:
		void onEditorValueChange() override;

		//Text field specific
		void toggleTyping();
		void beginTyping();
		void recordInput(const time::Time deltaTime);
		void updateString();
		void endTyping();///< Ends input receiving, storing any input that had been received until now

		bool stringUpdated = false;
		bool disableInputReceiveOnNextUpdate = false;
		bool multilineEditing = false;
		size_t maxStringLength;
		int typerPosition = 0;
		time::Time typerBlinkTime;
		time::Time typerBlinkTimer;
		std::string defaultString;
		std::string input;//Temporal storage for edited value so that value won't appear changed after each individual typed character
		se::rendering::Text* typeCharacter = nullptr;
		input::KeyboardRecorder keyboardRecorder;
	};
}