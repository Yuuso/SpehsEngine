#include "stdafx.h"
#include "SpehsEngine/Core/ScopeProfiler.h"

#include <map>
#include <mutex>
#include <thread>
#include <stack>


namespace
{
	// Shared static data
	std::recursive_mutex flushSignalMutex;
	boost::signals2::signal<void(const se::ScopeProfiler::ThreadData&)> flushSignal;

	// Thread local storage
	struct EnteredSection
	{
		std::string name;
		se::time::Time enterTime;
	};
	static thread_local se::ScopeProfiler::ThreadData threadData;
	static thread_local std::stack<se::ScopeProfiler::Section*> sectionStack;
	static thread_local se::time::Time lastFlushTime;

	void evaluateAutomaticFlush()
	{
		const se::time::Time now = se::time::now();
		const se::time::Time automaticFlushInterval = se::time::fromSeconds(1.0f / 60.0f);
		if (now - lastFlushTime > automaticFlushInterval || threadData.sections.size() > 10000)
		{
			if (flushSignalMutex.try_lock())
			{
				se::ScopeProfiler::flush();
				flushSignalMutex.unlock();
			}
		}
	}

	size_t getSectionCountRecursively(const std::map<se::time::Time, se::ScopeProfiler::Section>& sections)
	{
		size_t count = sections.size();
		for (const std::pair<const se::time::Time, se::ScopeProfiler::Section>& pair : sections)
		{
			count += getSectionCountRecursively(pair.second.children);
		}
		return count;
	};

	void removeFinishedSectionsRecursively(std::map<se::time::Time, se::ScopeProfiler::Section>& sections)
	{
		for (std::map<se::time::Time, se::ScopeProfiler::Section>::iterator it = sections.begin(); it != sections.end();)
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

	size_t removeFinishedSectionsRecursively(std::map<se::time::Time, se::ScopeProfiler::Section>& sections, const size_t maxRemoveCount)
	{
		size_t removeCount = 0u;

		for (std::map<se::time::Time, se::ScopeProfiler::Section>::iterator it = sections.begin(); it != sections.end();)
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

	void addSectionsRecursively(std::map<se::time::Time, se::ScopeProfiler::Section>& destination, const std::map<se::time::Time, se::ScopeProfiler::Section>& source)
	{
		for (const std::pair<const se::time::Time, se::ScopeProfiler::Section>& pair : source)
		{
			se::ScopeProfiler::Section& section = destination[pair.first];
			section.name = pair.second.name;
			section.function = pair.second.function;
			section.file = pair.second.file;
			section.line = pair.second.line;
			section.endTime = pair.second.endTime;
			addSectionsRecursively(section.children, pair.second.children);
		}
	}
}

namespace se
{
	void ScopeProfiler::enterSectionManually(const std::string_view name, const std::string_view function, const std::string_view file, const int line)
	{
#if SE_ENABLE_SCOPE_PROFILER == SE_TRUE
		// Add section
		const se::time::Time now = se::time::getProfilerTimestamp();
		se::ScopeProfiler::Section& section = sectionStack.empty() ? threadData.sections[now] : sectionStack.top()->children[now];
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
		const se::time::Time now = se::time::getProfilerTimestamp();
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
		ThreadData threadDataCopy = threadData;

		// Clear all excess data from thread local storage
		removeFinishedSectionsRecursively(threadData.sections);

		// Fire the flush signal
		std::lock_guard<std::recursive_mutex> lock(flushSignalMutex);
		flushSignal(threadDataCopy);
	}

	void ScopeProfiler::connectToFlushSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<void(const ThreadData&)>& callback)
	{
		std::lock_guard<std::recursive_mutex> lock(flushSignalMutex);
		scopedConnection = flushSignal.connect(callback);
	}

	size_t ScopeProfiler::Section::getDepth() const
	{
		size_t maxDepth = 0;
		for (const std::pair<time::Time, Section>& pair : children)
		{
			maxDepth = std::max(maxDepth, pair.second.getDepth());
		}
		return maxDepth + 1;
	}

	void ScopeProfiler::ThreadData::add(const ThreadData& other)
	{
		se_assert(threadId == other.threadId);
		addSectionsRecursively(sections, other.sections);
	}

	void ScopeProfiler::ThreadData::truncate(const size_t preferredSectionCount)
	{
		const size_t sectionCount = getSectionCountRecursively(sections);
		if (sectionCount > preferredSectionCount)
		{
			const size_t sectionsToRemove = sectionCount - preferredSectionCount;
			removeFinishedSectionsRecursively(sections, sectionsToRemove);
		}
	}

	size_t ScopeProfiler::ThreadData::getSectionCountRecursive() const
	{
		return getSectionCountRecursively(sections);
	}
}
