#include "stdafx.h"
#include "SpehsEngine/Net/SocketProfiler.h"

#include "SpehsEngine/Net/ISocket.h"

namespace se
{
	namespace net
	{
		SocketProfiler::SocketProfiler(const ISocket& _socket)
			: socket(_socket)
			, beginTime(time::now())
			, beginSentBytes(_socket.getSentBytes())
			, beginReceivedBytes(_socket.getReceivedBytes())
			, lastUpdateTime(time::now())
			, lastUpdateSentBytes(beginSentBytes)
			, lastUpdateReceivedBytes(beginReceivedBytes)
		{

		}

		void SocketProfiler::update()
		{
			const time::Time now = time::now();
			const size_t sentBytes = socket.getSentBytes();
			const size_t receivedBytes = socket.getReceivedBytes();
			updateFrames.push_back(UpdateFrame());
			updateFrames.back().duration = now - lastUpdateTime;
			updateFrames.back().sentBytes = sentBytes - lastUpdateSentBytes;
			updateFrames.back().receivedBytes = receivedBytes - lastUpdateReceivedBytes;
			lastUpdateTime = now;
			lastUpdateSentBytes = sentBytes;
			lastUpdateReceivedBytes = receivedBytes;

			//Calculate statistics
			time::Time duration;
			size_t totalSentBytes = 0;
			size_t totalReceivedBytes = 0;
			for (size_t i = 0; i < updateFrames.size(); i++)
			{
				duration += updateFrames[i].duration;
				totalSentBytes += updateFrames[i].sentBytes;
				totalReceivedBytes += updateFrames[i].receivedBytes;
			}
			sentBytesPerSecond = float(totalSentBytes) / duration.asSeconds();
			receivedBytesPerSecond = float(totalReceivedBytes) / duration.asSeconds();

			//Truncate overflowing update frames
			setUpdateFrameHistorySize(updateFrameHistorySize);
		}

		void SocketProfiler::setUpdateFrameHistorySize(const size_t _updateFrameHistorySize)
		{
			updateFrameHistorySize = _updateFrameHistorySize;
			if (updateFrames.size() > updateFrameHistorySize)
			{
				const size_t count = updateFrames.size() - updateFrameHistorySize;
				updateFrames.erase(updateFrames.begin(), updateFrames.begin() + count);
			}
		}
	}
}
