#include "ApplicationData.h"
#include "Polygon.h"
#include "BatchManager.h"
#include "InputManager.h"
#include "GUICheckbox.h"
#include "Text.h"


#define CHECKBOX_BORDER 2
#define SELECTED_ALPHA 1.0f
#define UNSELECTED_ALPHA 0.1f

namespace spehs
{
	GUICheckbox::GUICheckbox() : booleanPtr(nullptr), checkboxSize(20)
	{
		checkboxBackground = spehs::getActiveBatchManager()->createPolygon(Shape::BUTTON, 0, 1.0f, 1.0f);
		checkboxBackground->setColor(30, 30, 30);
		checkboxBackground->setCameraMatrixState(false);

		checkboxFilling = spehs::getActiveBatchManager()->createPolygon(Shape::BUTTON, 0, 1.0f, 1.0f);
		checkboxFilling->setColor(160, 170, 180);
		checkboxFilling->setCameraMatrixState(false);

		setJustification(GUIRECT_TEXT_JUSTIFICATION_LEFT);
	}
	GUICheckbox::GUICheckbox(int _ID) : GUICheckbox()
	{
		setID(_ID);
	}
	GUICheckbox::GUICheckbox(std::string str) : GUICheckbox()
	{
		setString(str);
	}
	GUICheckbox::~GUICheckbox()
	{
		checkboxBackground->destroy();
		checkboxFilling->destroy();
	}
	void GUICheckbox::update()
	{
		GUIRectangle::update();

		//Check mouse press
		previousSelectedState = checkBit(state, GUIRECT_SELECTED);
		if (getMouseHover() && inputManager->isKeyPressed(MOUSE_BUTTON_LEFT))
		{//Toggle selected state, update boolean pointer if one exists
			toggleState(GUIRECT_SELECTED);
			if (booleanPtr)
				*booleanPtr = checkBit(state, GUIRECT_SELECTED);
		}

		//Filling color
		if (checkBit(state, GUIRECT_SELECTED))
			checkboxFilling->setColorAlpha(SELECTED_ALPHA);
		else
			checkboxFilling->setColorAlpha(UNSELECTED_ALPHA);
	}
	void GUICheckbox::setRenderState(const bool _state)
	{
		GUIRectangle::setRenderState(_state);
		if (isVisible())
		{
			checkboxBackground->setRenderState(true);
			checkboxFilling->setRenderState(true);
		}
		else
		{
			checkboxBackground->setRenderState(false);
			checkboxFilling->setRenderState(false);
		}
	}
	void GUICheckbox::setDepth(uint16_t depth)
	{
		GUIRectangle::setDepth(depth);
		checkboxBackground->setPlaneDepth(depth + 1);
		checkboxFilling->setPlaneDepth(depth + 2);
	}
	void GUICheckbox::updatePosition()
	{
		//Checkbox has to exceptionally scale first before positioning
		updateScale();

		GUIRectangle::updatePosition();

		checkboxBackground->setPosition(getXGlobal() + size.x - CHECKBOX_BORDER - checkboxSize - applicationData->getWindowWidthHalf(), getYGlobal() + (size.y - checkboxSize) * 0.5f - applicationData->getWindowHeight() / 2);
		checkboxFilling->setPosition(getXGlobal() + size.x - checkboxSize - applicationData->getWindowWidthHalf(), getYGlobal() + (size.y - checkboxSize) * 0.5f + CHECKBOX_BORDER - applicationData->getWindowHeightHalf());

		//Update minimum size
		updateMinSize();
		
		//Text position
		if (text)
		{
			float textX = getXGlobal();
			if (checkBit(state, GUIRECT_TEXT_JUSTIFICATION_LEFT))
				textX += CHECKBOX_BORDER;
			else if (checkBit(state, GUIRECT_TEXT_JUSTIFICATION_RIGHT))
				textX += size.x - size.y - CHECKBOX_BORDER - text->getTextWidth() - TEXT_PREFERRED_SIZE_BORDER;
			else
				textX += 0.5f *(size.x - size.y - CHECKBOX_BORDER - text->getTextWidth());
			text->setPosition(textX, getYGlobal() + 0.5f * (size.y + text->getTextHeight()) - text->getFontHeight() - text->getFontDescender());
		}
	}
	void GUICheckbox::updateScale()
	{
		GUIRectangle::updateScale();

		checkboxBackground->resize(checkboxSize, checkboxSize);
		checkboxFilling->resize(checkboxSize - 2 * CHECKBOX_BORDER, checkboxSize - 2 * CHECKBOX_BORDER);
	}
	void GUICheckbox::updateMinSize()
	{
		minSize.x = 2 * CHECKBOX_BORDER + checkboxSize;
		if (text)
			minSize.x += text->getTextWidth();
		minSize.y = 2 * CHECKBOX_BORDER + checkboxSize;
		if (text && text->getTextHeight() > minSize.y)
			minSize.y = text->getTextHeight();
	}
	void GUICheckbox::setBooleanPtr(bool* ptr)
	{
		booleanPtr = ptr;
		if (booleanPtr)
		{
			if (*booleanPtr)
				enableBit(state, GUIRECT_SELECTED);
			else
				disableBit(state, GUIRECT_SELECTED);
		}
		else
			disableBit(state, GUIRECT_SELECTED);
	}
	bool GUICheckbox::selectedStateChanged()
	{
		if (checkState(GUIRECT_SELECTED) != previousSelectedState)
			return true;
		return false;
	}
}