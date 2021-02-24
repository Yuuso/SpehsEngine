#pragma once

#include "boost/signals2/connection.hpp"

namespace se
{
	namespace net
	{
		class ConnectionManager;
	}
	namespace imgui
	{
		class BackendWrapper;
	}
}


namespace se
{
	namespace debug
	{
		class ConnectionManagerVisualizer
		{
		public:

			ConnectionManagerVisualizer(net::ConnectionManager& connectionManager, imgui::BackendWrapper& imGuiBackendWrapper, const bool enabled);

			void setEnabled(const bool enabled);
			bool getEnabled() const { return preRenderConnection.connected(); }

		private:

			void render();

			net::ConnectionManager& connectionManager;
			imgui::BackendWrapper& imGuiBackendWrapper;

			boost::signals2::scoped_connection preRenderConnection;
		};
	}
}
