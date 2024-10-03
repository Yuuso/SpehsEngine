
#include "stdafx.h"
#include "SpehsEngine/Core/SE_Assert.h"

#if !defined(FINAL_RELEASE)
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SystemMessageBox.h"


namespace se
{
	bool AssertMessageBox(const std::string& _message, bool& _ignore)
	{
		log::info("Assert failed!\n" + _message, se::log::DARKRED);
		const MessageResult result = SystemMessageBox("Assert failed!", _message, BUTTONS_ABORT_RETRY_IGNORE | ICON_WARNING);
		switch (result)
		{
			default: [[fallthrough]];
			case MessageResult::RESULT_ERROR: [[fallthrough]];
			case MessageResult::RESULT_ABORT:
				// ABORT: Break program execution to debug
				return true;
			case MessageResult::RESULT_RETRY:
				// RETRY: Continue execution
				return false;
			case MessageResult::RESULT_IGNORE:
				// IGNORE: Continue execution and ignore these asserts in the future
				_ignore = true;
				return false;
		}
	}
}
#endif
