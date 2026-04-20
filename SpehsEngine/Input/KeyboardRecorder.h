#pragma once


namespace se::input
{
	class KeyboardRecorder
	{
	public:

		KeyboardRecorder(EventSignaler& _eventSignaler, const int _inputPriority, const std::u32string_view _string);
		virtual ~KeyboardRecorder() = default;

		// Returns index in output string where the next input will be instered into
		size_t getCursorIndex() const { return cursorIndex; }

		// Returns value only once finished
		std::optional<std::u32string> getResult() const { return finished ? std::make_optional(output) : std::nullopt; }
		const std::u32string& getOutput() const { return output; }

	private:

		std::u32string output;
		size_t cursorIndex = 0;
		bool finished = false;
		ScopedConnection textInputConnection;
		ScopedConnection keyboardEventConnection;
	};
}