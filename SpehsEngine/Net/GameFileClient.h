#pragma once

namespace se
{
	namespace net
	{
		class SocketTCP;
		class GameFileClient
		{
		public:
			GameFileClient(SocketTCP& socket);

			void update();

		private:
			SocketTCP& socket;
		};
	}
}
