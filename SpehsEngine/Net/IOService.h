#pragma once


namespace se
{
	namespace net
	{
		/*Upon creation, starts to run an IO service in a separate thread*/
		class IOService
		{
		public:

			struct Impl;

			IOService();
			~IOService();

		private:

			std::unique_ptr<Impl> impl;
		};
	}
}
