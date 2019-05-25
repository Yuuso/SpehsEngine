
#include "stdafx.h"
#include "SE_Assert.h"

#if !defined(FINAL_RELEASE)
#include "Log.h"
#include "SystemMessageBox.h"


namespace se
{
	bool AssertMessageBox(const std::string& _message, bool& _ignore)
	{
		se::log::info("Assert failed!\n" + _message, se::log::DARKRED);
		const MessageResult result = SystemMessageBox("Assert failed!", _message, BUTTONS_ABORT_RETRY_IGNORE | ICON_WARNING);
		switch (result)
		{
			default: [[fallthrough]];
			case se::MessageResult::RESULT_ERROR: [[fallthrough]];
			case se::MessageResult::RESULT_ABORT:
				// ABORT: Break program execution to debug
				return true;
			case se::MessageResult::RESULT_RETRY:
				// RETRY: Continue execution
				return false;
			case se::MessageResult::RESULT_IGNORE:
				// IGNORE: Continue execution and ignore these asserts in the future
				_ignore = true;
				return false;
		}
	}
}
#endif
