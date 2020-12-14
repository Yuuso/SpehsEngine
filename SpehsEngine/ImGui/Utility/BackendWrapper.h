#pragma once

#include "boost/signals2/connection.hpp"

namespace se
{
	namespace input
	{
		class EventSignaler;
	}

	namespace rendering
	{
		class Window;
	}
}


namespace se
{
	namespace imgui
	{
		class BackendWrapper
		{
		public:

			BackendWrapper(rendering::Window& window, input::EventSignaler& eventSignaler, const int inputPriority);
			~BackendWrapper();

			void beginNewFrame();
			void render();

			void setInputPriority(const int inputPriority);
			int getInputPriority() const { return inputPriority; }

		private:

			rendering::Window& window;
			input::EventSignaler& eventSignaler;

			bool beginNewFrameCalled = false;
			int inputPriority = 0;
			bool mousePressedStates[3] = { false, false, false };
            boost::signals2::scoped_connection mouseHoverConnection;
            boost::signals2::scoped_connection mouseWheelConnection;
            boost::signals2::scoped_connection mouseButtonPressedConnection;
            boost::signals2::scoped_connection textInputConnection;
            boost::signals2::scoped_connection keyboardKeyPressedConnection;
			boost::signals2::scoped_connection keyboardKeyPressedConnection0; // Max priority non-consuming
			boost::signals2::scoped_connection keyboardKeyReleasedConnection0; // Max priority non-consuming
		};
	}
}
