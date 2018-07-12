#pragma once
#include "SpehsEngine/GUI/GUIRectangle.h"


namespace se
{
	class GUIProgressBar : public GUIRectangle
	{
	public:
		GUIProgressBar(GUIContext& context);
		~GUIProgressBar();

		void updateMinSize() override;
		void updateScale() override;
		void updatePosition() override;

		void setProgressBarFrontColor(const se::Color& color);
		void setProgressBarFrontAlpha(const float a);
		void setProgressBarBackColor(const se::Color& color);
		void setProgressBarBackAlpha(const float a);

	private:
		se::Polygon* progressBarFront;
		se::Polygon* progressBarBack;
	};
}