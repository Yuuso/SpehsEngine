#pragma once


namespace se::input
{
	class KeyboardRecorder
	{
	public:

		KeyboardRecorder(EventSignaler& _eventSignaler, const int _inputPriority, const std::string_view _string);
		virtual ~KeyboardRecorder() = default;

		void setCharacterValidator(const std::function<bool(const char32_t)>& _isValidFunction) { isValidFunction = _isValidFunction; }

		// Returns index in output string where the next input will be inserted into
		size_t getCursorIndex() const { return cursorIndex; }

		// Returns value once finished, and resets the finished state.
		std::optional<std::string> popResult();
		const std::u32string& getOutput32() const { return output32; }

	private:

		// Store output in UTF32 -> this makes managing cursor much easier
		std::u32string output32;
		size_t cursorIndex = 0;
		bool finished = false;
		ScopedConnection textInputConnection;
		ScopedConnection keyboardEventConnection;
		std::function<bool(const char32_t)> isValidFunction;
	};
}