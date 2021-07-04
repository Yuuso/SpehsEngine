#pragma once

#include "boost/signals2.hpp"
#include "boost/signals2/connection.hpp"
#include "SpehsEngine/ImGui/ImGuiTypes.h"
#include <optional>

namespace se
{
	namespace input
	{
		class EventSignaler;
	}
	namespace graphics
	{
		class Renderer;
	}
}
struct ImFont;


namespace se
{
	namespace imgui
	{
		ImFont* getFont(const ImGuiFont _font);

		class BackendWrapper
		{
		public:

			BackendWrapper(input::EventSignaler& eventSignaler, const int inputPriority, graphics::Renderer& _renderer);
			~BackendWrapper();

			void render();

			void setInputPriority(const int inputPriority);
			int getInputPriority() const { return inputPriority; }

			void connectToPreRenderSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void()>& callback) { scopedConnection = preRenderSignal.connect(callback); }

		private:

			input::EventSignaler& eventSignaler;
			graphics::Renderer& renderer;

			int inputPriority = 0;
			bool mousePressedStates[3] = { false, false, false };
			std::optional<std::pair<std::string, float>> queuedFont;
            boost::signals2::scoped_connection eventSignalerPostUpdateConnection;
            boost::signals2::scoped_connection mouseHoverConnection;
            boost::signals2::scoped_connection mouseWheelConnection;
            boost::signals2::scoped_connection mouseButtonPressedConnection;
            boost::signals2::scoped_connection textInputConnection;
            boost::signals2::scoped_connection keyboardKeyPressedConnection;
			boost::signals2::scoped_connection keyboardKeyPressedConnection0; // Max priority non-consuming
			boost::signals2::scoped_connection keyboardKeyReleasedConnection0; // Max priority non-consuming
			boost::signals2::signal<void()> preRenderSignal;
		};
	}
}
