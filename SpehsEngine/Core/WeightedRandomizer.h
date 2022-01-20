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

	private:
		float totalWeight = 0.0f;
		std::vector<std::pair<float, T>> values;
	};
}