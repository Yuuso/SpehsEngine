#pragma once

#include "SpehsEngine/Math/Bounds2D.h"


namespace se
{
	template<typename T, int Capacity>
	struct Quadtree
	{
		static_assert(Capacity > 0, "Capacity must be larger than 0.");

		Quadtree(const Bounds2D& _bounds) : bounds(_bounds) {};
		Quadtree(const Quadtree<T, Capacity>& copy) = delete;
		Quadtree(const Quadtree<T, Capacity>&& move) = delete;

		~Quadtree()
		{
			for (size_t i = 0; i < 4; i++)
				delete children[i];
		}

		void add(const T& element, const Bounds2D elementBounds)
		{
			se_assert(bounds.contains(elementBounds.getCenter()));
			if (children[0])
			{
				//Recursion
				for (size_t i = 0; i < 4; i++)
				{
					if (children[i]->bounds.contains(elementBounds.getCenter()))
					{
						children[i]->add(element, elementBounds);
						return;
					}
				}
				se_assert(false && "Could not add element. Element isn't contained within any of the children.");
			}
			else
			{
				//Add
				elements.push_back(std::make_pair(element, elementBounds));

				//Split
				if (elements.size() > Capacity)
				{
					children[0] = new Quadtree<T, Capacity>(Bounds2D(bounds.center + 0.5f * glm::vec2(bounds.extents.x, bounds.extents.y), 0.5f * bounds.extents));
					children[1] = new Quadtree<T, Capacity>(Bounds2D(bounds.center + 0.5f * glm::vec2(-bounds.extents.x, bounds.extents.y), 0.5f * bounds.extents));
					children[2] = new Quadtree<T, Capacity>(Bounds2D(bounds.center + 0.5f * glm::vec2(bounds.extents.x, -bounds.extents.y), 0.5f * bounds.extents));
					children[3] = new Quadtree<T, Capacity>(Bounds2D(bounds.center + 0.5f * glm::vec2(-bounds.extents.x, -bounds.extents.y), 0.5f * bounds.extents));
					for (size_t e = 0; e < elements.size(); e++)
					{
						bool inserted = false;
						for (size_t c = 0; c < 4; c++)
						{
							if (children[c]->bounds.contains(elementBounds.getCenter()))
							{
								children[c]->add(element, elementBounds);
								inserted = true;
								break;
							}
						}
						se_assert(false && "Element could not be reallocated to any of the children.");
					}
					elements.clear();
				}
			}
		}

		std::vector<std::pair<T, Bounds2D>> elements = std::vector<std::pair<T, Bounds2D>>(Capacity);
		Bounds2D bounds;
		Quadtree<T, Capacity>* children[4];
	};
}
