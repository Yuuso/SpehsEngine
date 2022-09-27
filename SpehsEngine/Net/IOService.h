#pragma once


namespace se
{
	namespace net
	{
		/*Upon creation, starts to run an IO service in a separate thread*/
		class IOService
		{
		public:

			struct State;

			IOService();
			~IOService();

			State& getState() const { return *state; }

		private:

			void run();

			std::unique_ptr<State> state;
		};
	}
}