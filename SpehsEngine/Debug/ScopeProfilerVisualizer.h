#pragma once

#include "SpehsEngine/Core/ScopeProfiler.h"
#include "SpehsEngine/Graphics/View.h"
#include "SpehsEngine/Graphics/FontManager.h"
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/Graphics/Shape.h"
#include "SpehsEngine/Graphics/Text.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"
#include "SpehsEngine/Graphics/DefaultShaderManager.h"
#include <memory>
#include <mutex>


namespace se
{
	namespace debug
	{


		// --------------
		// TODO: With ImGui or GUI
		// --------------


		class ScopeProfilerVisualizer
		{
		public:

			ScopeProfilerVisualizer(graphics::View& _view, graphics::FontManager& _fontManager, graphics::DefaultShaderManager& _shaderManager, input::InputManager& _inputManager);
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
				const graphics::Shape* parent;
				const graphics::Shape* rootParent;
				time::Time beginTime;
				size_t depth = 0;
			};

			void profilerFlushCallback(const ScopeProfiler::ThreadData& threadData);
			void updateSectionPolygons();
			void updateSectionTextStrings();
			float getWidth();

			graphics::View& view;
			input::InputManager& inputManager;

			std::shared_ptr<graphics::TextMaterial> textMaterial;
			std::shared_ptr<graphics::FlatColorMaterial> shapeMaterial;

			// Visual settings
			float beginX = 5.0f;
			float beginY = 5.0f;
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
			std::unordered_map<const graphics::Shape*, SectionInfo> polygonToSectionInfoLookup;
			graphics::Text tooltipText;
			graphics::Shape tooltipPolygon;
			std::vector<std::unique_ptr<graphics::Text>> sectionTexts;
			std::vector<std::unique_ptr<graphics::Shape>> sectionPolygons;
			boost::signals2::scoped_connection profilerFlushConnection;

			std::recursive_mutex backgroundThreadDataMutex;
			size_t maxThreadDataSectionCount = 0;
			bool backgroundThreadDataUpdated = false;
			std::unordered_map<std::thread::id, ScopeProfiler::ThreadData> backgroundThreadDatas;
		};
	}
}
