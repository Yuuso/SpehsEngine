#pragma once
#include "GUIRectangleContainer.h"

namespace spehs
{
	/**An abstract base class for GUICRectangleContainers that have an uniform size across all the elements.*/
	class GUIRectangleUnisizeContainer : public GUIRectangleContainer
	{
	public:
		GUIRectangleUnisizeContainer() : elementSize(0, 0), minElementSize(0, 0) {}
		void clear() override
		{
			GUIRectangleContainer::clear();
			elementSize.x = 0;
			elementSize.x = 0;
			minElementSize.x = 0;
			minElementSize.y = 0;
		}

		/**The deriving class must update the container's minimum size according to the minimum element size.*/
		void updateMinSize() override = 0
		{
			minSize.x = 0;
			minSize.y = 0;
			minElementSize.x = 0;
			minElementSize.y = 0;
			for (unsigned i = 0; i < elements.size(); i++)
			{
				elements[i]->updateMinSize();
				if (elements[i]->getMinWidth() > minElementSize.x)
					minElementSize.x = elements[i]->getMinWidth();
				if (elements[i]->getMinHeight() > minElementSize.y)
					minElementSize.y = elements[i]->getMinHeight();
			}
			enableState(GUIRECT_MIN_SIZE_UPDATED_BIT);
		}

		/*These pure virtual methods MUST be implemented by the derived class*/
		virtual void updateScale() override = 0;
		virtual void updatePosition() override = 0;

	protected:
		glm::ivec2 elementSize;///< Current size of a single element, TO BE REMOVED (element size is dynamic as rectangle sizes are INTEGRAL, therefore uneven sizes result in malformed container dimensions. Extra size should be allocated somewhere (e.g. at the last element...))
		glm::ivec2 minElementSize;///< Minimum size of an elment (determined by the largest min width/height dimensions in the elements vector)
	};
}