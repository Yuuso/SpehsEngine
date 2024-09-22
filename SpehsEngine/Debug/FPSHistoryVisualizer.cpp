#include "stdafx.h"
#include "SpehsEngine/Debug/FPSHistoryVisualizer.h"

#include "SpehsEngine/ImGui/Utility/ImGuiUtility.h"
#include "SpehsEngine/ImGui/Utility/BackendWrapper.h"


namespace se
{
	namespace debug
	{
		void FPSHistoryVisualizerBase::setEnabled(const bool enabled)
		{
			if (preRenderConnection.connected() != enabled)
			{
				if (preRenderConnection.connected())
				{
					preRenderConnection.disconnect();
				}
				else
				{
					imGuiBackendWrapper.connectToPreRenderSignal(preRenderConnection, std::bind(&FPSHistoryVisualizerBase::render, this));
				}
			}
		}

		void FPSHistoryVisualizerBase::update()
		{
			if (beginFrameTime)
			{
				IStaticRingBuffer<float>& buffer = getStaticRingBuffer();
				const se::time::Time frameDuration = se::time::getProfilerTimestamp() - *beginFrameTime;
				if (buffer.isFull())
				{
					buffer.popFront();
				}
				buffer.pushBack(1.0f / frameDuration.asSeconds());

				// Precalculate common values
				const size_t size = buffer.getSize();
				const float averageMultiplier = 1.0f / float(size);
				const float firstValue = *buffer.getBuffer();
				bufferAverage = firstValue * averageMultiplier;
				bufferMin = firstValue;
				bufferMax = firstValue;
				for (size_t i = 1; i < size; i++)
				{
					const float value = *(buffer.getBuffer() + i);
					bufferMin = std::min(bufferMin, value);
					bufferMax = std::max(bufferMax, value);
					bufferAverage += value * averageMultiplier;
				}
			}
			beginFrameTime = se::time::getProfilerTimestamp();
		}

		void FPSHistoryVisualizerBase::render()
		{
			update();
			if (ImGui::Begin("FPS History"))
			{
				if (!beginFrameTime || *beginFrameTime - fpsStringUpdateTime > se::time::fromSeconds(0.5f))
				{
					fpsString = "FPS: ";
					fpsString += std::to_string(int(getAverage()));
					fpsStringUpdateTime = *beginFrameTime;
				}
				const IStaticRingBuffer<float>& buffer = getStaticRingBuffer();
				ImGui::PlotLines("FPS", buffer.getBuffer(), int(buffer.getSize()), int(buffer.getBeginOffset()), fpsString.c_str(), getMin(), getMax(), ImVec2(ImGui::GetWindowWidth(), 100.0f));
			}
			ImGui::End();
		}

		float FPSHistoryVisualizerBase::getAverageFrameSeconds()
		{
			const IStaticRingBuffer<float>& buffer = getStaticRingBuffer();
			const size_t size = buffer.getSize();
			if (size == 0)
			{
				return 0.0f;
			}
			const float multiplier = 1.0f / float(size);
			float average = 0.0f;
			for (size_t i = 0; i < size_t(size); i++)
			{
				const float* const pointer = buffer.getBuffer() + i;
				average += multiplier * (*pointer);
			}
			return average;
		}
	}
}
