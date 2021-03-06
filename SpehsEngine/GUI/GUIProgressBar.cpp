#include "stdafx.h"
#include "SpehsEngine/GUI/GUIProgressBar.h"
#include "SpehsEngine/Rendering/Polygon.h"
#include "SpehsEngine/Rendering/Text.h"

namespace se
{
	GUIProgressBar::GUIProgressBar(GUIContext& context)
		: GUIRectangle(context)
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

	void GUIProgressBar::setProgressBarFrontColor(const se::Color& _color)
	{
		progressBarFront->setColor(_color);
	}

	void GUIProgressBar::setProgressBarFrontAlpha(const float a)
	{
		progressBarFront->setAlpha(a);
	}

	void GUIProgressBar::setProgressBarBackColor(const se::Color& _color)
	{
		progressBarBack->setColor(_color);
	}

	void GUIProgressBar::setProgressBarBackAlpha(const float a)
	{
		progressBarBack->setAlpha(a);
	}
}