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

			void render();

			void setInputPriority(const int inputPriority);
			int getInputPriority() const { return inputPriority; }

			void setFont(const std::string_view filepath, const float fontSize);

			void connectToPreRenderSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void()>& callback) { scopedConnection = preRenderSignal.connect(callback); }

		private:

			rendering::Window& window;
			input::EventSignaler& eventSignaler;

			int inputPriority = 0;
			bool mousePressedStates[3] = { false, false, false };
			std::optional<std::pair<std::string, float>> queuedFont;
            boost::signals2::scoped_connection eventSignalerPostUpdateConnection;
            boost::signals2::scoped_connection mouseHoverConnection;
            boost::signals2::scoped_connection mouseWheelConnection;
            boost::signals2::scoped_connection mouseButtonConnection;
            boost::signals2::scoped_connection textInputConnection;
            boost::signals2::scoped_connection keyboardConnection;
			boost::signals2::scoped_connection keyboardConnection0; // Max priority non-consuming
			boost::signals2::signal<void()> preRenderSignal;
		};
	}
}
