#pragma once


namespace se
{
	namespace net
	{
		class SocketProfiler
		{
		public:
			SocketProfiler(const ISocket& socket);

			/* Update needs to be called continuously over time so that data can be collected. */
			void update();

			void setUpdateFrameHistorySize(const size_t historySize);

			float getSentBytesPerSecond() const { return sentBytesPerSecond; }
			float getReceivedBytesPerSecond() const { return receivedBytesPerSecond; }

		private:
			const ISocket& socket;
			const time::Time beginTime;
			const size_t beginSentBytes;
			const size_t beginReceivedBytes;
			time::Time lastUpdateTime;
			size_t lastUpdateSentBytes = 0;
			size_t lastUpdateReceivedBytes = 0;
			struct UpdateFrame
			{
				time::Time duration;
				size_t sentBytes = 0;
				size_t receivedBytes = 0;
			};
			std::vector<UpdateFrame> updateFrames;
			size_t updateFrameHistorySize = 30;
			float sentBytesPerSecond = 0.0f;
			float receivedBytesPerSecond = 0.0f;
		};
	}
}
