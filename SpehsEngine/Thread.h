#pragma once
namespace std
{
	class thread;
}
namespace spehs
{
	void setThreadName(const char* threadName);
	void setThreadName(std::thread* thread, const char* threadName);
}