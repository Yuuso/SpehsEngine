#pragma once

#include <string>

namespace se
{
	namespace log
	{
		enum TextColor : int
		{
			NONE			= -1,
			BLACK			= 0,
			DARKBLUE		/*= FOREGROUND_BLUE*/,
			DARKGREEN		/*= FOREGROUND_GREEN*/,
			DARKCYAN		/*= FOREGROUND_GREEN | FOREGROUND_BLUE*/,
			DARKRED			/*= FOREGROUND_RED*/,
			DARKMAGENTA		/*= FOREGROUND_RED | FOREGROUND_BLUE*/,
			DARKYELLOW		/*= FOREGROUND_RED | FOREGROUND_GREEN*/,
			GRAY			/*= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE*/,
			DARKGRAY		/*= FOREGROUND_INTENSITY*/,
			BLUE			/*= FOREGROUND_INTENSITY | FOREGROUND_BLUE*/,
			GREEN			/*= FOREGROUND_INTENSITY | FOREGROUND_GREEN*/,
			CYAN			/*= FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE*/,
			RED				/*= FOREGROUND_INTENSITY | FOREGROUND_RED*/,
			MAGENTA			/*= FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE*/,
			YELLOW			/*= FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN*/,
			WHITE			/*= FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE*/
		};

		void info(const std::string_view message, const TextColor color);
		void info(const std::string_view message);
		void warning(const std::string_view message);
		void error(const std::string_view message);
	}
}