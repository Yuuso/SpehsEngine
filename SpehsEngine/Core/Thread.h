#pragma once

namespace std
{
	class thread;
}


namespace se
{
	void setThreadName(const std::string_view _name);
	void setThreadName(std::thread& _thread, const std::string_view _name);
}
