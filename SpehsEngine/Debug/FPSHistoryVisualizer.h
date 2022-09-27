#pragma once

#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/StaticRingBuffer.h"
#include "SpehsEngine/ImGui/Utility/ImGuiUtility.h"
#include "SpehsEngine/ImGui/Utility/BackendWrapper.h"
#include "SpehsEngine/ImGui/imgui.h"
#include "boost/signals2/connection.hpp"


namespace se
{
	namespace debug
	{
		template<uint32_t MaxSize>
		class FPSHistoryVisualizer
		{
		public:

			FPSHistoryVisualizer(imgui::BackendWrapper& _imGuiBackendWrapper, const bool _renderEnabled)
				: imGuiBackendWrapper(_imGuiBackendWrapper)
			{
				setEnabled(_renderEnabled);
			}

			void setEnabled(const bool enabled)
			{
				if (preRenderConnection.connected() == enabled)
				{
					return;
				}

				if (preRenderConnection.connected())
				{
					preRenderConnection.disconnect();
				}
				else
				{
					imGuiBackendWrapper.connectToPreRenderSignal(preRenderConnection, boost::bind(&FPSHistoryVisualizer<MaxSize>::render, this));
				}
			}

			bool getEnabled() const { return preRenderConnection.connected(); }

			float getAverageFrameSeconds()
			{
				const size_t size = staticRingBuffer.getSize();
				if (size == 0)
				{
					return 0.0f;
				}
				const float multiplier = 1.0f / float(size);
				float average = 0.0f;
				for (size_t i = 0; i < size_t(size); i++)
				{
					average += multiplier * staticRingBuffer[i];
				}
				return average;
			}

			const StaticRingBuffer<float, MaxSize>& getStaticRingBuffer() const { return staticRingBuffer; }
			float getMin() const { return bufferMin; }
			float getMax() const { return bufferMax; }
			float getAverage() const { return bufferAverage; }

		private:

			void update()
			{
				if (beginFrameTime)
				{
					const se::time::Time frameDuration = se::time::getProfilerTimestamp() - *beginFrameTime;
					if (staticRingBuffer.isFull())
					{
						staticRingBuffer.popFront();
					}
					staticRingBuffer.pushBack(1.0f / frameDuration.asSeconds());

					// Precalculate common values
					const size_t size = staticRingBuffer.getSize();
					const float averageMultiplier = 1.0f / float(size);
					bufferAverage = staticRingBuffer[0] * averageMultiplier;
					bufferMin = staticRingBuffer[0];
					bufferMax = staticRingBuffer[0];
					for (size_t i = 1; i < size; i++)
					{
						const float value = staticRingBuffer[i];
						bufferMin = std::min(bufferMin, value);
						bufferMax = std::max(bufferMax, value);
						bufferAverage += value * averageMultiplier;
					}
				}
				beginFrameTime = se::time::getProfilerTimestamp();
			}

			void render()
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
					ImGui::PlotLines("FPS", staticRingBuffer.getBuffer(), int(staticRingBuffer.getSize()), int(staticRingBuffer.getBeginOffset()), fpsString.c_str(), getMin(), getMax(), ImVec2(ImGui::GetWindowWidth(), 100.0f));
				}
				ImGui::End();
			}

			StaticRingBuffer<float, MaxSize> staticRingBuffer; // Stores values of fps. This odd format was selected to support the ImGui graph rendering that requires an array of floats.
			imgui::BackendWrapper& imGuiBackendWrapper;
			float bufferMin = 0.0f;
			float bufferMax = 0.0f;
			float bufferAverage = 0.0f;
			std::string fpsString;
			se::time::Time fpsStringUpdateTime;
			std::optional<se::time::Time> beginFrameTime;

			boost::signals2::scoped_connection preRenderConnection;
		};
	}
}
