#pragma once


namespace se
{
	namespace log
	{
		enum class LogType : uint8_t
		{
			Debug,
			Info,
			Warning,
			Error,
			Fatal,
		};

		void connectToLogSignal(ScopedConnection& scopedConnection, const std::function<void(const std::string_view, const LogType)>& callback);
	}
}
