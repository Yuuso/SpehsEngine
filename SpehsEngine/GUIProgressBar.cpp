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
	void GUIProgressBar::setProgressBarFrontColor(const glm::vec3& color)
	{
		progressBarFront->setColor(color);
	}
	void GUIProgressBar::setProgressBarFrontColor(const glm::vec4& color)
	{
		progressBarFront->setColor(color);
	}
	void GUIProgressBar::setProgressBarFrontColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
	{
		progressBarFront->setColor(r, g, b, a);
	}
	void GUIProgressBar::setProgressBarFrontColor(const float r, const float g, const float b, const float a)
	{
		progressBarFront->setColor(r, g, b, a);
	}
	void GUIProgressBar::setProgressBarFrontColorAlpha(const unsigned char a)
	{
		progressBarFront->setColorAlpha(a);
	}
	void GUIProgressBar::setProgressBarFrontColorAlpha(const float a)
	{
		progressBarFront->setColorAlpha(a);
	}
	void GUIProgressBar::setProgressBarBackColor(const glm::vec3& color)
	{
		progressBarBack->setColor(color);
	}
	void GUIProgressBar::setProgressBarBackColor(const glm::vec4& color)
	{
		progressBarBack->setColor(color);
	}
	void GUIProgressBar::setProgressBarBackColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
	{
		progressBarBack->setColor(r, g, b, a);
	}
	void GUIProgressBar::setProgressBarBackColor(const float r, const float g, const float b, const float a)
	{
		progressBarBack->setColor(r, g, b, a);
	}
	void GUIProgressBar::setProgressBarBackColorAlpha(const unsigned char a)
	{
		progressBarBack->setColorAlpha(a);
	}
	void GUIProgressBar::setProgressBarBackColorAlpha(const float a)
	{
		progressBarBack->setColorAlpha(a);
	}
}