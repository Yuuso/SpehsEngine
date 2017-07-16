#pragma once
#include "GUIRectangle.h"


namespace spehs
{
	class GUIProgressBar : public GUIRectangle
	{
	public:
		GUIProgressBar();
		~GUIProgressBar();

		void updateMinSize() override;
		void updateScale() override;
		void updatePosition() override;

		void setProgressBarFrontColor(const spehs::Color& color);
		void setProgressBarFrontAlpha(const spehs::Color::Component& a);
		void setProgressBarBackColor(const spehs::Color& color);
		void setProgressBarBackAlpha(const spehs::Color::Component& a);

	private:
		spehs::Polygon* progressBarFront;
		spehs::Polygon* progressBarBack;
	};
}