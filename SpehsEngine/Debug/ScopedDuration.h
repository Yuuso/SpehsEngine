#pragma once


namespace se
{
	namespace debug
	{
		class ScopedDuration
		{
		public:
			ScopedDuration(const char* const _message);
			ScopedDuration(ScopedDuration&& move);
			ScopedDuration(const ScopedDuration& copy) = delete;
			~ScopedDuration();
		private:

			struct Data
			{
				Data(const char* const _message)
					: message(_message)
				{
				}
				const char* const message;
				time::Time duration;
				std::unordered_map<const char*, std::unique_ptr<Data>> children;
			};

			//TODO: prevent construction with 'new'?

			static thread_local std::unique_ptr<Data> root;
			static thread_local std::vector<Data*> stack;
			Data* data = nullptr;
			const time::Time beginTime;
		};
	}
}

#define SE_CONCAT(a, b) SE_CONCAT_INNER(a, b)
#define SE_CONCAT_INNER(a, b) a ## b
#define SE_COUNTER_SUFFIX(p_Name) SE_CONCAT(p_Name, __COUNTER__)
#if SE_CONFIGURATION != SE_FINAL_RELEASE
#define SE_SCOPED_DURATION(p_Message) se::debug::ScopedDuration SE_COUNTER_SUFFIX(p_scopedDuration_)(p_Message); do {} while(false)
#define SE_SCOPED_DURATION_FUNC() se::debug::ScopedDuration SE_COUNTER_SUFFIX(p_scopedDuration_)(__FUNCTION__); do {} while(false)
#else
#define SE_SCOPED_DURATION(p_Message) do {} while(false)
#define SE_SCOPED_DURATION_FUNC() do {} while(false)
#endif
