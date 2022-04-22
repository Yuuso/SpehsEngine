#pragma once

#include <optional>


namespace se
{
	// Contains individually weighted values of type T. Allows retrieving one value by weighted random.
	template<typename T>
	class WeightedRandomizer
	{
	public:

		void add(const float weight, const T& t)
		{
			if (weight > 0.0f)
			{
				totalWeight += weight;
				values.push_back(std::make_pair(totalWeight, t));
			}
		}

		template<typename SeedType>
		const T* get(se::rng::PRNG<SeedType>& rng) const
		{
			if (!values.empty())
			{
				const float roll = rng.random<float>(0.0f, totalWeight);
				for (const std::pair<float, T>& pair : values)
				{
					if (roll <= pair.first)
					{
						return &pair.second;
					}
				}
			}
			return nullptr;
		}

		template<typename SeedType>
		const std::optional<size_t> getIndex(se::rng::PRNG<SeedType>& rng) const
		{
			if (!values.empty())
			{
				const float roll = rng.random<float>(0.0f, totalWeight);
				for (size_t i = 0; i < values.size(); i++)
				{
					const std::pair<float, T>& pair = values[i];
					if (roll <= pair.first)
					{
						return i;
					}
				}
			}
			return std::nullopt;
		}

		const T* getWithIndex(const size_t index) const
		{
			if (index < values.size())
			{
				return &values[index].second;
			}
			else
			{
				return nullptr;
			}
		}

		void eraseIndex(const size_t index)
		{
			if (index < values.size())
			{
				values.erase(values.begin() + index);
			}
		}

		size_t size() const { return values.size(); }
		bool empty() const { return values.empty(); }

	private:
		float totalWeight = 0.0f;
		std::vector<std::pair<float, T>> values;
	};
}