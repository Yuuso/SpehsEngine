#pragma once
#include "SpehsEngine/GUI/GUIRectangleContainer.h"

namespace spehs
{
	/** A simple container where all the elements are aligned in a straight column.
	Option for element unisize functionality. */
	class GUIRectangleColumn : public GUIRectangleContainer
	{
	public:
		GUIRectangleColumn(BatchManager& _batchManager);
		~GUIRectangleColumn() override;

		void setUniformHeight(const bool _state);
		void updateMinSize() override;
		void updateScale() override;
		void updatePosition() override;

	protected:
		bool uniformHeight;
	};
}