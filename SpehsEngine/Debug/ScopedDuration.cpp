#include "stdafx.h"
#include "SpehsEngine/Debug/ScopedDuration.h"


namespace se
{
	namespace debug
	{
		thread_local std::unique_ptr<ScopedDuration::Data> ScopedDuration::root;
		thread_local std::vector<ScopedDuration::Data*> ScopedDuration::stack;

		ScopedDuration::ScopedDuration(const char* const _message)
			: beginTime(getProfilerTime())
		{
			if (stack.empty())
			{
				se_assert(!root);
				root.reset(new Data(_message));
				data = root.get();
			}
			else
			{
				std::unique_ptr<Data>& child = stack.back()->children[_message];
				if (!child)
				{
					child.reset(new Data(_message));
				}
				data = child.get();
			}
			stack.push_back(data);
		}

		ScopedDuration::ScopedDuration(ScopedDuration&& move)
			: data(move.data)
			, beginTime(move.beginTime)
		{
			move.data = nullptr;
		}

		ScopedDuration::~ScopedDuration()
		{
			if (data)
			{
				se_assert(data == stack.back());
				data->duration += getProfilerTime() - beginTime;
				stack.pop_back();
				if (stack.empty())
				{
					// Log stack
					std::function<void(const ScopedDuration::Data&, const Time* const, const size_t)> log;
					log = [&log](const ScopedDuration::Data& data, const Time* const parentDuration, const size_t depth)
					{
						std::string string(depth, ' ');
						string += std::string(data.message) + " " + std::to_string(int(data.duration.asMilliseconds())) + " ms";
						if (parentDuration)
						{
							string += std::string(" (") + toString(100.0f * data.duration.asSeconds() / parentDuration->asSeconds(), 2) + "%)";
						}
						log::info(string);
						for (const std::pair<const char* const, std::unique_ptr<ScopedDuration::Data>>& pair : data.children)
						{
							log(*pair.second, &data.duration, depth + 1);
						}
					};
					log(*data, nullptr, 0);
					se_assert(root.get() == data);
					root.reset();
				}
				else
				{
					se_assert(root.get() != data);
				}
			}
		}
	}
}