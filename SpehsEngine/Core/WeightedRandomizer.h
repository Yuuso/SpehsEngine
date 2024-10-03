#pragma once


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
				values.push_back(std::make_pair(weight, t));
			}
		}

		template<typename SeedType>
		const T* get(rng::PRNG<SeedType>& rng) const
		{
			if (!values.empty())
			{
				float roll = rng.random<float>(0.0f, totalWeight);
				for (const std::pair<float, T>& pair : values)
				{
					if (roll <= pair.first)
					{
						return &pair.second;
					}
					else
					{
						roll -= pair.first;
					}
				}
				return &values.back().second;
			}
			return nullptr;
		}

		template<typename SeedType>
		const std::optional<size_t> getIndex(rng::PRNG<SeedType>& rng) const
		{
			if (!values.empty())
			{
				float roll = rng.random<float>(0.0f, totalWeight);
				for (size_t i = 0; i < values.size(); i++)
				{
					const std::pair<float, T>& pair = values[i];
					if (roll <= pair.first)
					{
						return i;
					}
					else
					{
						roll -= pair.first;
					}
				}
				return values.size() - 1;
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
				totalWeight = 0.0f;
				for (const std::pair<float, T>& pair : values)
				{
					totalWeight += pair.first;
				}
			}
		}

		size_t size() const { return values.size(); }
		bool empty() const { return values.empty(); }

	private:
		float totalWeight = 0.0f;
		std::vector<std::pair<float, T>> values;
	};
}