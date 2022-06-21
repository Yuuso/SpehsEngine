#pragma once

#include <functional>

namespace boost
{
	namespace signals2
	{
		class scoped_connection;
	}
}


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

		void connectToLogSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(const std::string_view, const LogType)>& callback);
	}
}