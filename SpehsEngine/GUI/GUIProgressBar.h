#pragma once
#include "SpehsEngine/GUI/GUIRectangle.h"


namespace spehs
{
	class GUIProgressBar : public GUIRectangle
	{
	public:
		GUIProgressBar(GUIContext& context);
		~GUIProgressBar();

		void updateMinSize() override;
		void updateScale() override;
		void updatePosition() override;

		void setProgressBarFrontColor(const spehs::Color& color);
		void setProgressBarFrontAlpha(const float a);
		void setProgressBarBackColor(const spehs::Color& color);
		void setProgressBarBackAlpha(const float a);

	private:
		spehs::Polygon* progressBarFront;
		spehs::Polygon* progressBarBack;
	};
}