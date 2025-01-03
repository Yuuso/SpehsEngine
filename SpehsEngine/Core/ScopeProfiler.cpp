#include "stdafx.h"
#include "SpehsEngine/Core/ScopeProfiler.h"

#include "SpehsEngine/Core/ScopeProfilerTypes.h"
#include <stack>


namespace se
{
	namespace
	{
		// Shared static data
		std::recursive_mutex flushSignalMutex;
		Signal<void(const ScopeProfilerThreadData&)> flushSignal;

		// Thread local storage
		struct EnteredSection
		{
			std::string name;
			Time enterTime;
		};
		static thread_local ScopeProfilerThreadData threadData;
		static thread_local std::stack<ScopeProfilerSection*> sectionStack;
		static thread_local Time lastFlushTime;

		void evaluateAutomaticFlush()
		{
			const Time now = getEpochTime();
			const Time automaticFlushInterval = Time::fromSeconds(1.0f / 60.0f);
			if (now - lastFlushTime > automaticFlushInterval || threadData.sections.size() > 10000)
			{
				if (flushSignalMutex.try_lock())
				{
					ScopeProfiler::flush();
					flushSignalMutex.unlock();
				}
			}
		}

		size_t getSectionCountRecursively(const std::map<Time, ScopeProfilerSection>& sections)
		{
			size_t count = sections.size();
			for (const std::pair<const Time, ScopeProfilerSection>& pair : sections)
			{
				count += getSectionCountRecursively(pair.second.children);
			}
			return count;
		};

		void removeFinishedSectionsRecursively(std::map<Time, ScopeProfilerSection>& sections)
		{
			for (std::map<Time, ScopeProfilerSection>::iterator it = sections.begin(); it != sections.end();)
			{
				if (it->second.endTime)
				{
					it = sections.erase(it);
				}
				else
				{
					removeFinishedSectionsRecursively(it->second.children);
					it++;
				}
			}
		}

		size_t removeFinishedSectionsRecursively(std::map<Time, ScopeProfilerSection>& sections, const size_t maxRemoveCount)
		{
			size_t removeCount = 0u;

			for (std::map<Time, ScopeProfilerSection>::iterator it = sections.begin(); it != sections.end();)
			{
				// Recurse towards leaf and start removing from there
				removeCount += removeFinishedSectionsRecursively(it->second.children, maxRemoveCount - removeCount);
				se_assert(removeCount <= maxRemoveCount);

				if (it->second.children.empty() && it->second.endTime && maxRemoveCount - removeCount > 0)
				{
					it = sections.erase(it);
					removeCount++;
				}
				else
				{
					it++;
				}
			}

			return removeCount;
		}

		void addSectionsRecursively(std::map<Time, ScopeProfilerSection>& destination, const std::map<Time, ScopeProfilerSection>& source)
		{
			for (const std::pair<const Time, ScopeProfilerSection>& pair : source)
			{
				ScopeProfilerSection& section = destination[pair.first];
				section.name = pair.second.name;
				section.function = pair.second.function;
				section.file = pair.second.file;
				section.line = pair.second.line;
				section.endTime = pair.second.endTime;
				addSectionsRecursively(section.children, pair.second.children);
			}
		}
	}

	void ScopeProfiler::enterSectionManually(const std::string_view name, const std::string_view function, const std::string_view file, const int line)
	{
#if SE_ENABLE_SCOPE_PROFILER == SE_TRUE
		// Add section
		const Time now = getProfilerTime();
		ScopeProfilerSection& section = sectionStack.empty() ? threadData.sections[now] : sectionStack.top()->children[now];
		section.name = name;
		section.function = function;
		section.file = file;
		section.line = line;
		sectionStack.push(&section);
#endif
	}

	void ScopeProfiler::leaveSectionManually()
	{
#if SE_ENABLE_SCOPE_PROFILER == SE_TRUE
		// Finish section
		const Time now = getProfilerTime();
		se_assert(!sectionStack.empty());
		sectionStack.top()->endTime.emplace(now);
		sectionStack.pop();
		evaluateAutomaticFlush();
#endif
	}

	ScopeProfiler::ScopeProfiler(const std::string_view _name, const std::string_view _function, const std::string_view _file, const int _line)
	{
		enterSectionManually(_name, _function, _file, _line);
	}

	ScopeProfiler::~ScopeProfiler()
	{
		leaveSectionManually();
	}

	void ScopeProfiler::flush()
	{
		// Update thread data
		threadData.threadId = std::this_thread::get_id();

		// Copy thread data to non-thread local memory
		ScopeProfilerThreadData threadDataCopy = threadData;

		// Clear all excess data from thread local storage
		removeFinishedSectionsRecursively(threadData.sections);

		// Fire the flush signal
		std::lock_guard<std::recursive_mutex> lock(flushSignalMutex);
		flushSignal(threadDataCopy);
	}

	void ScopeProfiler::connectToFlushSignal(ScopedConnection& scopedConnection, const std::function<void(const ScopeProfilerThreadData&)>& callback)
	{
		std::lock_guard<std::recursive_mutex> lock(flushSignalMutex);
		scopedConnection = flushSignal.connect(callback);
	}

	size_t ScopeProfilerSection::getDepth() const
	{
		size_t maxDepth = 0;
		for (const std::pair<Time, ScopeProfilerSection>& pair : children)
		{
			maxDepth = std::max(maxDepth, pair.second.getDepth());
		}
		return maxDepth + 1;
	}

	void ScopeProfilerThreadData::add(const ScopeProfilerThreadData& other)
	{
		se_assert(threadId == other.threadId);
		addSectionsRecursively(sections, other.sections);
	}

	void ScopeProfilerThreadData::truncate(const size_t preferredSectionCount)
	{
		const size_t sectionCount = getSectionCountRecursively(sections);
		if (sectionCount > preferredSectionCount)
		{
			const size_t sectionsToRemove = sectionCount - preferredSectionCount;
			removeFinishedSectionsRecursively(sections, sectionsToRemove);
		}
	}

	size_t ScopeProfilerThreadData::getSectionCountRecursive() const
	{
		return getSectionCountRecursively(sections);
	}
}
