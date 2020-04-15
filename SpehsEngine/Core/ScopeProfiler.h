#pragma once
#include "SpehsEngine/Core/SE_Time.h"
#include "boost/signals2.hpp"
#include "boost/function.hpp"
#include <memory>
#include <thread>
#include <optional>


namespace se
{
	class ScopeProfiler
	{
	public:

		struct Section
		{
			size_t getDepth() const;
			std::string name;
			std::string_view function;
			std::string_view file;
			int line = 0;
			std::optional<se::time::Time> endTime;
			std::map<se::time::Time, Section> children;
		};

		struct ThreadData
		{
			void add(const ThreadData& other);
			void truncate(const size_t preferredSectionCount);
			size_t getSectionCountRecursive() const;

			std::thread::id threadId;
			std::map<se::time::Time, Section> sections;
		};

		/*
			functionName must point to static memory.
		*/
		ScopeProfiler(const std::string_view name, const std::string_view function, const std::string_view file, const int line);
		~ScopeProfiler();

		/*
			Dynamic allocation is not allowed.
			ScopeProfiler objects are meant to be created on stack so that their destruction order is the reverse of the construction order.
		*/
		void* operator new(size_t size) = delete;

		/*
			Can be called from any thread.
			Triggers the flush signal and passes all thread local data as the signal parameter.
			Clears all finished sections from the thread local storage.
		*/
		static void flush();

		/*
			Can be called from any thread.
			Callback can be made from any thread at any time.
			An ending thread has no obligation to call flush before it dies out, therefore potentially leaving some thread data unflushed.
			Inactive threads that do not make calls to push() or flush() may also hold onto potential stored thread data.
		*/
		static void connectToFlushSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<void(const ThreadData&)>& callback);

		/*
			Always prefer using ScopeProfiler instance over this.
			Calling section entering/leaving in incorrect order will mess the sections up and potentially crash the program.
		*/
		static void enterSectionManually(const std::string_view name, const std::string_view function, const std::string_view file, const int line);
		static void leaveSectionManually();
	};
}

#if SE_ENABLE_SCOPE_PROFILER == SE_TRUE
#define SE_SCOPE_PROFILER_1(p_name, ...) se::ScopeProfiler p_scopeProfiler##p_name(std::string(__VA_ARGS__), __FUNCTION__, __FILE__, __LINE__); do {} while (false)
#define SE_SCOPE_PROFILER_2(p_counter, ...) SE_SCOPE_PROFILER_1(p_counter, __VA_ARGS__)
#define SE_SCOPE_PROFILER(...) SE_SCOPE_PROFILER_2(__COUNTER__, __VA_ARGS__)
#else
#define SE_SCOPE_PROFILER_1(p_name, ...) do {} while(false)
#define SE_SCOPE_PROFILER_2(p_counter, ...) do {} while(false)
#define SE_SCOPE_PROFILER(...) do {} while(false)
#endif
