#pragma once
namespace std
{
	class thread;
}
namespace se
{
	void setThreadName(const char* threadName);
	void setThreadName(std::thread* thread, const char* threadName);
}