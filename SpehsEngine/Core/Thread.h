#pragma once
#include <string>
#include <thread>
namespace se
{
	void setThreadName(const std::string& name);
	void setThreadName(std::thread& thread, const std::string& name);
}
