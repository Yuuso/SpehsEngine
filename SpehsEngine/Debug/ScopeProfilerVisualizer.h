#pragma once
#include "SpehsEngine/Core/ScopeProfiler.h"
#include <memory>
#include <mutex>


namespace se
{
	namespace rendering
	{
		class Text;
		class Polygon;
	}
	class GUIContext;

	namespace debug
	{
		class ScopeProfilerVisualizer
		{
		public:

			ScopeProfilerVisualizer(GUIContext& guiContext);
			~ScopeProfilerVisualizer();

			void update(const time::Time& deltaTime);

			void setRenderState(const bool visible);
			bool getRenderState() const;

			void setEnableUpdate(const bool enabled);
			bool getEnableUpdate() const;

			/* Set an override for preferred root section length. For example at 1/60 of a second when the root section is the frame. Used for section colouring purposes. */
			void setTargetRootSectionWidth(const time::Time* const time);
			void setTimeWindowWidth(const time::Time& time);
			void translateTimeWindowBegin(const time::Time& time);
			void setMaxThreadDataSectionCount(const size_t count);

		private:

			struct SectionInfo
			{
				const ScopeProfiler::Section* section = nullptr;
				rendering::Polygon* parent = nullptr;
				rendering::Polygon* rootParent = nullptr;
				time::Time beginTime;
				size_t depth = 0;
			};

			void profilerFlushCallback(const ScopeProfiler::ThreadData& threadData);
			void updateSectionPolygons();
			void updateSectionTextStrings();

			GUIContext& guiContext;

			// Visual settings
			float beginX = 5.0f;
			float beginY = 5.0f;
			float width = 0.0f;
			float sectionHeight = 32.0f;
			float horizontalSpeed = 1.0f;
			time::Time timeWindowBegin; // Timestamp where the time window begins
			time::Time timeWindowWidth = time::fromSeconds(1.0f); // Visualized duration
			time::Time disableUpdateTime;
			std::optional<time::Time> targetRootSectionWidth;

			bool renderState = true;
			bool enableUpdate = true;
			bool firstVisualUpdateDone = false;

			std::thread::id activeThreadId;
			std::unordered_map<std::thread::id, ScopeProfiler::ThreadData> threadDatas;
			std::unordered_map<rendering::Polygon*, SectionInfo> polygonToSectionInfoLookup;
			rendering::Text& tooltipText;
			rendering::Polygon& tooltipPolygon;
			std::vector<rendering::Text*> sectionTexts;
			std::vector<rendering::Polygon*> sectionPolygons;

			std::recursive_mutex backgroundThreadDataMutex;
			size_t maxThreadDataSectionCount = 0;
			bool backgroundThreadDataUpdated = false;
			std::unordered_map<std::thread::id, ScopeProfiler::ThreadData> backgroundThreadDatas;

			// Signal binds: declare last to destruct first
			boost::signals2::scoped_connection profilerFlushConnection;
		};
	}
}
