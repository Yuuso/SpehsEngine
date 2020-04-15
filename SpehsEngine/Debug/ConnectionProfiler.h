#pragma once

#include "SpehsEngine/Net/Connection.h"
#include <memory>


namespace se
{
	namespace rendering
	{
		class Text;
	}
	class GUIContext;

	namespace debug
	{
		class ConnectionProfiler
		{
		public:
			ConnectionProfiler(GUIContext& guiContext);

			void addConnection(std::shared_ptr<net::Connection>& connection);

			/* Update needs to be called continuously over time so that data can be collected. */
			void update();

			void setTextColor(const Color& color);

		private:

			GUIContext& guiContext;

			std::vector<std::shared_ptr<net::Connection>> connections;
			rendering::Text& text;
			boost::signals2::scoped_connection incomingConnectionConnection;
		};
	}
}
