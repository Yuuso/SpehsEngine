#include "ApplicationData.h"
#include "PolygonBatch.h"
#include "InputManager.h"
#include "GUICheckbox.h"
#include "Text.h"
#define CHECKBOX_BORDER 2
#define SELECTED_ALPHA 1.0f
#define UNSELECTED_ALPHA 0.1f

namespace SpehsEngine
{
	GUICheckbox::GUICheckbox() : booleanPtr(nullptr)
	{
		checkboxBackground = new PolygonBatch(Shape::BUTTON);
		checkboxBackground->setColor(30, 30, 30);
		checkboxBackground->setCameraMatrixState(false);
		checkboxFilling = new PolygonBatch(Shape::BUTTON);
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
		delete checkboxBackground;
		delete checkboxFilling;
	}
	void GUICheckbox::update()
	{
		GUIRectangle::update();

		//Check mouse press
		if (getMouseHover() && inputManager->isKeyPressed(MOUSE_BUTTON_LEFT))
		{//Toggle selected state, update boolean pointer if one exists
			toggleState(GUIRECT_SELECTED);
			if (booleanPtr)
				*booleanPtr = checkBit(state, GUIRECT_SELECTED);
		}

		//Filling color
		if (checkBit(state, GUIRECT_SELECTED))
			checkboxFilling->setAlpha(SELECTED_ALPHA);
		else
			checkboxFilling->setAlpha(UNSELECTED_ALPHA);
	}
	void GUICheckbox::render()
	{
		GUIRectangle::render();
		checkboxBackground->draw();
		checkboxFilling->draw();
	}
	void GUICheckbox::updatePosition()
	{
		//Checkbox has to exceptionally scale first before positioning
		updateScale();

		GUIRectangle::updatePosition();

		checkboxBackground->setPosition(getX() + size.x - CHECKBOX_BORDER - checkboxWidth - applicationData->getWindowWidth() / 2, getY() + size.y / 2.0f - checkboxWidth / 2.0f - applicationData->getWindowHeight() / 2);
		checkboxFilling->setPosition(getX() + size.x - checkboxWidth - applicationData->getWindowWidthHalf(), getY() + size.y / 2.0f - checkboxWidth / 2.0f + CHECKBOX_BORDER - applicationData->getWindowHeightHalf());

		//Update minimum size
		updateMinSize();
		
		//Text position
		float textX = getX();
		if (checkBit(state, GUIRECT_TEXT_JUSTIFICATION_LEFT))
			textX += CHECKBOX_BORDER;
		else if (checkBit(state, GUIRECT_TEXT_JUSTIFICATION_RIGHT))
			textX += size.x - size.y - CHECKBOX_BORDER - text->getTextWidth() - TEXT_PREFERRED_SIZE_BORDER;
		else
			textX += 0.5f *(size.x - size.y - CHECKBOX_BORDER - text->getTextWidth());
		text->setPosition(textX, getY() + 0.5f * (size.y + text->getTextHeight()) - text->getFontHeight() - text->getFontDescender());

	}
	void GUICheckbox::updateScale()
	{
		GUIRectangle::updateScale();

		checkboxWidth = size.y - 2 * CHECKBOX_BORDER;
		if (checkboxWidth > size.x - text->getTextWidth() - CHECKBOX_BORDER)
			checkboxWidth = size.x - text->getTextWidth() - CHECKBOX_BORDER;

		checkboxBackground->resize(checkboxWidth, checkboxWidth);
		checkboxFilling->resize(checkboxWidth - 2 * CHECKBOX_BORDER, checkboxWidth - 2 * CHECKBOX_BORDER);
	}
	void GUICheckbox::updateMinSize()
	{
		GUIRectangle::updateMinSize();
		minSize.x += (text->getTextHeight() + CHECKBOX_BORDER);
	}
	void GUICheckbox::setBooleanPtr(bool* ptr)
	{
		booleanPtr = ptr;
		if (*booleanPtr)
			enableBit(state, GUIRECT_SELECTED);
		else
			disableBit(state, GUIRECT_SELECTED);
	}
}