#pragma once

#include "SpehsEngine/Core/StaticRingBuffer.h"


namespace se
{
	namespace debug
	{
		class FPSHistoryVisualizerBase
		{
		public:
			virtual ~FPSHistoryVisualizerBase() = default;
			void setEnabled(const bool enabled);
			bool getEnabled() const { return preRenderConnection.isConnected(); }
			float getMin() const { return bufferMin; }
			float getMax() const { return bufferMax; }
			float getAverage() const { return bufferAverage; }
			
		protected:
			// Only for the inheriting class
			FPSHistoryVisualizerBase(imgui::BackendWrapper& _imGuiBackendWrapper, const bool _renderEnabled)
				: imGuiBackendWrapper(_imGuiBackendWrapper)
			{
				setEnabled(_renderEnabled);
			}
			void update();
			virtual IStaticRingBuffer<float>& getStaticRingBuffer() = 0;
			virtual const IStaticRingBuffer<float>& getStaticRingBuffer() const = 0;
			imgui::BackendWrapper& imGuiBackendWrapper;
			float bufferMin = 0.0f;
			float bufferMax = 0.0f;
			float bufferAverage = 0.0f;
			std::string fpsString;
			time::Time fpsStringUpdateTime;
			std::optional<time::Time> beginFrameTime;

		private:
			void render();
			float getAverageFrameSeconds();
			ScopedConnection preRenderConnection;
		};

		template<uint32_t MaxSize>
		class FPSHistoryVisualizer final : public FPSHistoryVisualizerBase
		{
		public:

			FPSHistoryVisualizer(imgui::BackendWrapper& _imGuiBackendWrapper, const bool _renderEnabled)
				: FPSHistoryVisualizerBase(_imGuiBackendWrapper, _renderEnabled)
			{
			}
			IStaticRingBuffer<float>& getStaticRingBuffer() { return staticRingBuffer; }
			const IStaticRingBuffer<float>& getStaticRingBuffer() const { return staticRingBuffer; }

		private:

			StaticRingBuffer<float, MaxSize> staticRingBuffer; // Stores values of fps. This odd format was selected to support the ImGui graph rendering that requires an array of floats.
		};
	}
}
