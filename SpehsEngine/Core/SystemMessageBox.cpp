
#include "stdafx.h"
#include "SystemMessageBox.h"

#if defined(_WIN32)
#include <Windows.h>
#endif


namespace se
{
	MessageResult SystemMessageBox(const std::string& _caption, const std::string& _message, const unsigned int _flags)
	{
#if defined(_WIN32)
		return (MessageResult)MessageBox(NULL, _message.c_str(), _caption.c_str(), _flags);
#else
#error SystemMessageBox not implemented
		// Note: SDL has a message box
		return MessageResult::RESULT_ERROR;
#endif
	}
}
