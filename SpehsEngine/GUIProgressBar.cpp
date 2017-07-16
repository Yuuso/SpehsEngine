#include "GUIProgressBar.h"
#include "Polygon.h"
#include "Text.h"

namespace spehs
{
	GUIProgressBar::GUIProgressBar()
	{
	}
	GUIProgressBar::~GUIProgressBar()
	{
		progressBarFront->destroy();
		progressBarBack->destroy();
	}
	void GUIProgressBar::updateMinSize()
	{
		enableState(GUIRECT_MIN_SIZE_UPDATED_BIT);
		disableState(GUIRECT_SCALE_UPDATED_BIT);
	}
	void GUIProgressBar::updateScale()
	{
		enableState(GUIRECT_SCALE_UPDATED_BIT);
		disableState(GUIRECT_POSITION_UPDATED_BIT);
	}
	void GUIProgressBar::updatePosition()
	{
		enableState(GUIRECT_POSITION_UPDATED_BIT);
	}
	//PROGRESS BAR COLOR SETTING
	void GUIProgressBar::setProgressBarFrontColor(const spehs::Color& color)
	{
		progressBarFront->setColor(color);
	}
	void GUIProgressBar::setProgressBarFrontAlpha(const spehs::Color::Component& a)
	{
		progressBarFront->setAlpha(a);
	}
	void GUIProgressBar::setProgressBarBackColor(const spehs::Color& color)
	{
		progressBarBack->setColor(color);
	}
	void GUIProgressBar::setProgressBarBackAlpha(const spehs::Color::Component& a)
	{
		progressBarBack->setAlpha(a);
	}
}