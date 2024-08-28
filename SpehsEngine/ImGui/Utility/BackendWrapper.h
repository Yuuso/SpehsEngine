#pragma once

#include "SpehsEngine/ImGui/ImGuiTypes.h"

namespace se
{
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
            boost::signals2::scoped_connection mouseButtonConnection;
            boost::signals2::scoped_connection textInputConnection;
            boost::signals2::scoped_connection keyboardConnection;
			boost::signals2::scoped_connection keyboardConnection0; // Max priority non-consuming
			boost::signals2::signal<void()> preRenderSignal;
		};
	}
}
