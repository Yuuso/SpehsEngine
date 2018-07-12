#pragma once
#include <thread>
#include <string>

namespace se
{
	namespace device
	{
		class Display4x8
		{
		public:
			Display4x8();
			~Display4x8();

			void setString(const std::string& str);

		private:
			std::thread* runThread;

		};
	}
}