#include "stdafx.h"
#include "SpehsEngine/Debug/ScopeProfilerVisualizer.h"

#include "SpehsEngine/Graphics/Camera.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"
#include "SpehsEngine/Graphics/Scene.h"
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/Input/Key.h"
#include "SpehsEngine/Input/MouseUtilityFunctions.h"
#include <sstream>


namespace se
{
	namespace debug
	{
		ScopeProfilerVisualizer::ScopeProfilerVisualizer(graphics::View& _view, graphics::FontManager& _fontManager, graphics::ShaderManager& _shaderManager, input::InputManager& _inputManager)
			: view(_view)
			, inputManager(_inputManager)
			, activeThreadId(std::this_thread::get_id())
		{
			setMaxThreadDataSectionCount(64);
			ScopeProfiler::connectToFlushSignal(profilerFlushConnection, boost::bind(&ScopeProfilerVisualizer::profilerFlushCallback, this, boost::placeholders::_1));

			se_assert(_view.getCamera().getProjection() == graphics::Projection::Orthographic);

			textMaterial = graphics::createMaterial(graphics::DefaultMaterialType::Text, _shaderManager);
			textMaterial->setFont(_fontManager.getDefaultFont());

			shapeMaterial = graphics::createMaterial(graphics::DefaultMaterialType::FlatColor, _shaderManager);

			tooltipPolygon.generate(graphics::ShapeType::Rectangle);
			tooltipPolygon.setMaterial(shapeMaterial);
			tooltipPolygon.setColor(Color(0.2f, 0.2f, 0.2f));
			_view.getScene().add(tooltipPolygon);

			tooltipText.setMaterial(textMaterial);
			tooltipText.setColor(Color());
			_view.getScene().add(tooltipText);
		}

		ScopeProfilerVisualizer::~ScopeProfilerVisualizer()
		{
			// Disconnect profiler flush connection, then acquire the background thread data mutex to make sure that no background thread is currently in the signal callback.
			{
				profilerFlushConnection.disconnect();
				std::lock_guard<std::recursive_mutex> lock(backgroundThreadDataMutex);
			}
		}

		float ScopeProfilerVisualizer::getWidth()
		{
			return view.getSize().width - 10.0f;
		}

		void ScopeProfilerVisualizer::update(const time::Time& deltaTime)
		{
			SE_SCOPE_PROFILER();

			if (inputManager.isKeyPressed(unsigned(input::Key::KP_DIVIDE)))
			{
				setRenderState(!getRenderState());
			}
			if (inputManager.isKeyPressed(unsigned(input::Key::KP_MULTIPLY)))
			{
				setEnableUpdate(!getEnableUpdate());
			}
			if (inputManager.isKeyPressed(unsigned(input::Key::KP_MINUS)))
			{
				setTimeWindowWidth(std::max(1ll, timeWindowWidth.value << 1));
			}
			if (inputManager.isKeyPressed(unsigned(input::Key::KP_PLUS)))
			{
				setTimeWindowWidth(std::max(1ll, timeWindowWidth.value >> 1));
			}
			if (inputManager.isKeyDown(unsigned(input::Key::KP_4)))
			{
				translateTimeWindowBegin(-horizontalSpeed * deltaTime.asSeconds() * timeWindowWidth);
			}
			if (inputManager.isKeyDown(unsigned(input::Key::KP_6)))
			{
				translateTimeWindowBegin(horizontalSpeed * deltaTime.asSeconds() * timeWindowWidth);
			}
			if (inputManager.isKeyPressed(unsigned(input::Key::KP_5)))
			{
				timeWindowBegin = enableUpdate ? time::getProfilerTimestamp() : disableUpdateTime;
				translateTimeWindowBegin(-timeWindowWidth / 2);
			}
			if (inputManager.isKeyPressed(unsigned(input::Key::KP_0)))
			{
				if (!threadDatas.empty())
				{
					std::unordered_map<std::thread::id, ScopeProfilerThreadData>::const_iterator it = threadDatas.find(activeThreadId);
					if (it != threadDatas.end())
					{
						it++;
					}
					if (it == threadDatas.end())
					{
						it = threadDatas.begin();
					}
					activeThreadId = it->first;

					// Update if update is disabled
					if (!enableUpdate)
					{
						std::lock_guard<std::recursive_mutex> lock(backgroundThreadDataMutex);
						if (backgroundThreadDataUpdated)
						{
							backgroundThreadDataUpdated = false;
							threadDatas = backgroundThreadDatas;
							updateSectionPolygons();
						}
					}
				}
			}

			if (renderState)
			{
				// Update thread data?
				if (enableUpdate)
				{
					timeWindowBegin += deltaTime;

					std::lock_guard<std::recursive_mutex> lock(backgroundThreadDataMutex);
					if (backgroundThreadDataUpdated)
					{
						backgroundThreadDataUpdated = false;
						threadDatas = backgroundThreadDatas;
						updateSectionPolygons();
					}
				}

				if (!firstVisualUpdateDone)
				{
					firstVisualUpdateDone = true;
					timeWindowBegin = time::now() - timeWindowWidth;
				}

				const std::unordered_map<std::thread::id, ScopeProfilerThreadData>::const_iterator threadDataIt = threadDatas.find(activeThreadId);
				const ScopeProfilerThreadData* const threadData = threadDataIt != threadDatas.end() ? &threadDataIt->second : nullptr;

				// Element update
				const time::Time now = enableUpdate ? time::getProfilerTimestamp() : disableUpdateTime;
				const time::Time endTime = timeWindowBegin + timeWindowWidth; // Visualized history ends
				const time::Time beginTime = endTime - timeWindowWidth; // Visualized history begins

				tooltipText.setRenderState(false);
				tooltipPolygon.setRenderState(false);
				for (auto&& sectionPolygon : sectionPolygons)
				{
					auto it = polygonToSectionInfoLookup.find(sectionPolygon.get());
					if (it == polygonToSectionInfoLookup.end())
					{
						log::error("Polygon key not found from the polygonToSectionInfoLookup.");
						continue;
					}

					const SectionInfo& sectionInfo = it->second;
					if (sectionInfo.beginTime > endTime)
					{
						sectionPolygon->setRenderState(false);
						continue;
					}

					const ScopeProfilerSection& section = *sectionInfo.section;
					const time::Time sectionEndTime = section.endTime ? *section.endTime : now;
					if (sectionEndTime < beginTime)
					{
						sectionPolygon->setRenderState(false);
						continue;
					}

					auto parentIt = polygonToSectionInfoLookup.find(sectionInfo.parent);
					const time::Time parentSectionBeginTime = parentIt != polygonToSectionInfoLookup.end()
						? parentIt->second.beginTime
						: time::Time::zero;
					const time::Time parentSectionEndTime = parentIt != polygonToSectionInfoLookup.end()
						? (parentIt->second.section->endTime ? *parentIt->second.section->endTime : now)
						: time::Time::zero;
					const time::Time parentSectionDuration = parentSectionEndTime - parentSectionBeginTime;
					auto rootParentIt = polygonToSectionInfoLookup.find(sectionInfo.rootParent);
					const time::Time rootParentSectionBeginTime = rootParentIt != polygonToSectionInfoLookup.end()
						? rootParentIt->second.beginTime
						: time::Time::zero;
					const time::Time rootParentSectionEndTime = rootParentIt != polygonToSectionInfoLookup.end()
						? (rootParentIt->second.section->endTime ? *rootParentIt->second.section->endTime : now)
						: time::Time::zero;
					const time::Time rootParentSectionDuration = rootParentSectionEndTime - rootParentSectionBeginTime;
					const time::Time visualCompareSectionDuration = targetRootSectionWidth ? *targetRootSectionWidth : rootParentSectionDuration;
					const time::Time sectionBeginTime = sectionInfo.beginTime;
					const time::Time sectionDuration = sectionEndTime - sectionBeginTime;
					const time::Time displaySectionBeginTime = std::max(beginTime, sectionBeginTime);
					const time::Time displaySectionEndTime = std::min(endTime, sectionEndTime);
					const time::Time displaySectionDuration = displaySectionEndTime - displaySectionBeginTime;
					se_assert(displaySectionDuration >= time::Time::zero);
					const float xStartPercentage = (displaySectionBeginTime - beginTime).asSeconds() / timeWindowWidth.asSeconds();
					const float xEndPercentage = (displaySectionEndTime - beginTime).asSeconds() / timeWindowWidth.asSeconds();
					const float timeWindowPercentage = xEndPercentage - xStartPercentage;
					const glm::vec2 position(beginX + getWidth() * xStartPercentage, beginY + sectionInfo.depth * sectionHeight);
					const glm::vec2 size(std::max(1.0f, timeWindowPercentage * getWidth()), sectionHeight);
					const float percentageOfParent = sectionInfo.parent ? (sectionDuration.asSeconds() / parentSectionDuration.asSeconds()) : 0.0f;
					const float percentageOfVisualCompareSection = sectionDuration.asSeconds() / visualCompareSectionDuration.asSeconds();
					const float positiveColorFactor = percentageOfVisualCompareSection * 0.25f;
					sectionPolygon->setRenderState(true);
					sectionPolygon->setPosition({ position.x, 0.0f, position.y });
					sectionPolygon->setScale({ size.x, 1.0f, size.y });
					sectionPolygon->setColor(Color(1.0f, 0.0f, 0.0f) * positiveColorFactor + Color(0.0f, 1.0f, 0.0f) * (1.0f - positiveColorFactor));

					// Tooltip hover?
					if (const std::optional<glm::vec2> mousePosition = input::getMousePositionf())
					{
						if (mousePosition->x >= position.x && mousePosition->x <= (position.x + size.x) &&
							mousePosition->y >= position.y && mousePosition->y <= (position.y + size.y))
						{
							std::string string;
							string += "Name: " + section.name;
							string += "\nFunction: " + std::string(section.function);
							string += "\nFile: " + std::string(section.file);
							string += "\nLine: " + std::to_string(section.line);
							string += "\nLength: " + toTimeLengthString(sectionDuration, 6);
							if (sectionInfo.parent)
							{
								string += " (" + toString(percentageOfParent * 100.0f, 2) + "% of parent)";
							}
							if (threadData)
							{
								std::stringstream stringstream;
								stringstream << threadData->threadId;
								string += "\nThread id: " + std::string(stringstream.str());
							}
							tooltipText.clear();
							tooltipText.insert(string);
							tooltipText.setRenderState(true);
							const float textWidth = tooltipText.getDimensions().dimensions.x;
							const float tooltipPolygonBorder = 2.0f;
							const float tooltipWidth = textWidth + 2.0f * tooltipPolygonBorder;
							const glm::vec2 tooltipTextPosition = glm::vec2(std::min(mousePosition->x + tooltipWidth, view.getSize().width) - tooltipWidth, mousePosition->y);
							const glm::vec2 tooltipPolygonPosition = tooltipTextPosition + glm::vec2(tooltipPolygonBorder, tooltipPolygonBorder);
							tooltipText.setPosition({ tooltipTextPosition.x, 0.0f, tooltipTextPosition.y });
							tooltipPolygon.setRenderState(true);
							tooltipPolygon.setScale({ tooltipWidth, 1.0f, tooltipText.getDimensions().dimensions.y + 2.0f * tooltipPolygonBorder });
							tooltipPolygon.setPosition({ tooltipPolygonPosition.x, 0.0f, tooltipPolygonPosition.y });
						}
					}
				}
			}
		}

		void ScopeProfilerVisualizer::updateSectionPolygons()
		{
			SE_SCOPE_PROFILER();

			polygonToSectionInfoLookup.clear();
			const std::unordered_map<std::thread::id, ScopeProfilerThreadData>::const_iterator it = threadDatas.find(activeThreadId);
			if (it != threadDatas.end())
			{
				const ScopeProfilerThreadData& threadData = it->second;

				// Generate section polygons
				const size_t sectionCount = threadData.getSectionCountRecursive();
				if (sectionPolygons.size() > sectionCount)
				{
					const size_t removeCount = sectionPolygons.size() - sectionCount;
					sectionPolygons.erase(sectionPolygons.end() - removeCount, sectionPolygons.end());
				}
				else
				{
					while (sectionPolygons.size() < sectionCount)
					{
						sectionPolygons.push_back(std::make_unique<graphics::Shape>());
						sectionPolygons.back()->generate(graphics::ShapeType::Rectangle);
						sectionPolygons.back()->setColor(Color(0.0f, 1.0f, 0.0f));
					}
				}

				if (!threadData.sections.empty())
				{
					size_t sectionIndex = 0u;
					std::function<void(const time::Time, const ScopeProfilerSection&, const size_t, const graphics::Shape* const, const graphics::Shape* const)> updatePolygon;
					updatePolygon =
						[&updatePolygon, &sectionIndex, this](const time::Time sectionBeginTime, const ScopeProfilerSection& section, const size_t depth, const graphics::Shape* const parent, const graphics::Shape* const rootParent)
						{
							const graphics::Shape* polygon = sectionPolygons[sectionIndex++].get();
							SectionInfo& sectionInfo = polygonToSectionInfoLookup[polygon];
							sectionInfo.section = &section;
							sectionInfo.parent = parent;
							sectionInfo.rootParent = rootParent;
							sectionInfo.beginTime = sectionBeginTime;
							sectionInfo.depth = depth;
							for (const std::pair<const time::Time, ScopeProfilerSection>& pair : section.children)
							{
								updatePolygon(pair.first, pair.second, depth + 1, polygon, rootParent ? rootParent : polygon);
							}
						};

					size_t depth = 0;
					for (const std::pair<const time::Time, ScopeProfilerSection>& pair : threadData.sections)
					{
						updatePolygon(pair.first, pair.second, depth, nullptr, nullptr);
					}
					while (sectionIndex < sectionPolygons.size())
					{
						sectionPolygons[sectionIndex++]->setRenderState(false);
					}
				}
			}
			else
			{
				sectionPolygons.clear();
			}
		}

		void ScopeProfilerVisualizer::updateSectionTextStrings()
		{
			//std::stringstream stringstream;
			//stringstream << "Threads: " + std::to_string(threadDatas.size());
			//for (const std::pair<std::thread::id, Profiler::ThreadData> threadDataPair : threadDatas)
			//{
			//	stringstream << "\n    Thread id: " << threadDataPair.first;
			//	for (const std::pair<time::Time, Profiler::Section>& sectionPair : threadDataPair.second.sections)
			//	{
			//		const time::Time duration = sectionPair.second.endTime - sectionPair.first;
			//		stringstream << "\n        " << sectionPair.second.name << ": " << std::to_string(duration.asNanoseconds()) << " ns";
			//	}
			//}

			//text.setString(stringstream.str());
		}

		void ScopeProfilerVisualizer::setTargetRootSectionWidth(const time::Time* const time)
		{
			if (time)
			{
				targetRootSectionWidth.emplace(*time);
			}
			else
			{
				targetRootSectionWidth.reset();
			}
		}

		void ScopeProfilerVisualizer::setTimeWindowWidth(const time::Time& time)
		{
			if (time != timeWindowWidth)
			{
				const time::Time delta = time - timeWindowWidth;
				const time::Time halfDelta = delta / 2ll;
				translateTimeWindowBegin(-halfDelta);
				timeWindowWidth = time;
			}
		}

		void ScopeProfilerVisualizer::translateTimeWindowBegin(const time::Time& time)
		{
			const time::Time newTimeWindowBegin = timeWindowBegin + time;
			if (time < time::Time::zero)
			{
				if (newTimeWindowBegin < timeWindowBegin)
				{
					timeWindowBegin = newTimeWindowBegin;
				}
				else
				{
					timeWindowBegin.value = std::numeric_limits<decltype(timeWindowBegin.value)>::min();
				}
			}
			else
			{
				if (newTimeWindowBegin > timeWindowBegin)
				{
					timeWindowBegin = newTimeWindowBegin;
				}
				else
				{
					timeWindowBegin.value = std::numeric_limits<decltype(timeWindowBegin.value)>::max();
				}
			}
		}

		void ScopeProfilerVisualizer::setMaxThreadDataSectionCount(const size_t count)
		{
			if (maxThreadDataSectionCount != count)
			{
				maxThreadDataSectionCount = count;
			}
		}

		void ScopeProfilerVisualizer::setRenderState(const bool visible)
		{
			renderState = visible;
			tooltipText.setRenderState(visible);
			for (auto&& sectionText : sectionTexts)
			{
				sectionText->setRenderState(visible);
			}
			if (!visible)
			{
				tooltipText.setRenderState(visible);
				tooltipPolygon.setRenderState(visible);
				for (auto&& sectionPolygon : sectionPolygons)
				{
					sectionPolygon->setRenderState(visible);
				}
			}
		}

		bool ScopeProfilerVisualizer::getRenderState() const
		{
			return renderState;
		}

		void ScopeProfilerVisualizer::setEnableUpdate(const bool enabled)
		{
			if (enabled != enableUpdate)
			{
				enableUpdate = enabled;
				if (enabled)
				{
					const time::Time timeSinceDisableUpdate = time::now() - disableUpdateTime;
					timeWindowBegin += timeSinceDisableUpdate;
				}
				else
				{
					disableUpdateTime = time::now();
				}
			}
		}

		bool ScopeProfilerVisualizer::getEnableUpdate() const
		{
			return enableUpdate;
		}

		void ScopeProfilerVisualizer::profilerFlushCallback(const ScopeProfilerThreadData& _threadData)
		{
			std::lock_guard<std::recursive_mutex> lock(backgroundThreadDataMutex);
			std::unordered_map<std::thread::id, ScopeProfilerThreadData>::iterator it = backgroundThreadDatas.find(_threadData.threadId);
			if (it != backgroundThreadDatas.end())
			{
				it->second.add(_threadData);
			}
			else
			{
				backgroundThreadDatas[_threadData.threadId] = _threadData;
			}
			backgroundThreadDatas[_threadData.threadId].truncate(3000);
			backgroundThreadDataUpdated = true;
		}
	}
}
