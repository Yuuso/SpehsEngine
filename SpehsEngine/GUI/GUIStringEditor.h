#pragma once
#include "SpehsEngine/Input/KeyboardRecorder.h"
#include "SpehsEngine/GUI/GUIRectangle.h"
#include "SpehsEngine/GUI/ValueEditor.h"
#include <string>
#include <vector>

namespace spehs
{
	class GUIStringEditor : public GUIRectangle, public ValueEditor<std::string>
	{
		static int defaultMaxStringEditorStringLength;
	public:
		GUIStringEditor(BatchManager& _batchManager);
		~GUIStringEditor() override;

		void setRenderState(const bool _state) override;
		void setDepth(const int16_t depth) override;
		void setString(const std::string& str) override;
		void setStringSize(const int size) override;
		void setStringSizeRelative(const int relativeSize) override;
		void setStringColor(const spehs::Color& col) override;
		void setStringAlpha(const spehs::Color::Component& a) override;
		void updatePosition() override;
		void updateMinSize() override;
		void inputUpdate(InputUpdateData& data) override;
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

		bool stringUpdated;
		bool disableInputReceiveOnNextUpdate;
		bool multilineEditing;
		int maxStringLength;
		int typerPosition;
		time::Time typerBlinkTime;
		time::Time typerBlinkTimer;
		std::string defaultString;
		std::string input;//Temporal storage for edited value so that value won't appear changed after each individual typed character
		spehs::Text* typeCharacter;
		KeyboardRecorder keyboardRecorder;
	};
}