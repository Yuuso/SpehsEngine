#pragma once

#include "SpehsEngine/ImGui/ImGuiTypes.h"

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

			void connectToPreRenderSignal(ScopedConnection& scopedConnection, const std::function<void()>& callback) { scopedConnection = preRenderSignal.connect(callback); }

		private:

			input::EventSignaler& eventSignaler;
			graphics::Renderer& renderer;

			ScopedConnections inputPriorityConnections;
			int inputPriority = 0;
			bool mousePressedStates[3] = { false, false, false };
			std::optional<std::pair<std::string, float>> queuedFont;
            ScopedConnection eventSignalerPostUpdateConnection;
			ScopedConnection maxPriorityKeyboardConnection;
			Signal<void()> preRenderSignal;
		};
	}
}
