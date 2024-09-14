#pragma once

#include <thread>


namespace se
{
	struct ScopeProfilerSection
	{
		size_t getDepth() const;
		std::string name;
		std::string_view function;
		std::string_view file;
		int line = 0;
		std::optional<se::time::Time> endTime;
		std::map<se::time::Time, ScopeProfilerSection> children;
	};

	struct ScopeProfilerThreadData
	{
		void add(const ScopeProfilerThreadData& other);
		void truncate(const size_t preferredSectionCount);
		size_t getSectionCountRecursive() const;

		std::thread::id threadId;
		std::map<se::time::Time, ScopeProfilerSection> sections;
	};
}
