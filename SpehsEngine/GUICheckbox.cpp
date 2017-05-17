#include "ApplicationData.h"
#include "Polygon.h"
#include "InputManager.h"
#include "GUICheckbox.h"
#include "Text.h"


#define CHECKBOX_BORDER 2
#define SELECTED_ALPHA 1.0f
#define UNSELECTED_ALPHA 0.1f

namespace spehs
{
	GUICheckbox::GUICheckbox(const bool checkboxStateEnabled) : ValueEditor(checkboxStateEnabled), checkboxSize(20)
	{
		checkboxBackground = spehs::Polygon::create(Shape::BUTTON, getDepth() + 1, 1.0f, 1.0f);
		checkboxBackground->setColor(30, 30, 30);
		checkboxBackground->setCameraMatrixState(false);

		checkboxFilling = spehs::Polygon::create(Shape::BUTTON, getDepth() + 2, 1.0f, 1.0f);
		checkboxFilling->setColor(160, 170, 180);
		checkboxFilling->setCameraMatrixState(false);

		onEditorValueChange();
		setJustification(GUIRECT_TEXT_JUSTIFICATION_LEFT_BIT);
	}
	GUICheckbox::GUICheckbox(const GUIRECT_ID_TYPE _ID) : GUICheckbox(false)
	{
		setID(_ID);
	}
	GUICheckbox::GUICheckbox(const char* str) : GUICheckbox(false)
	{
		setString(str);
	}
	GUICheckbox::~GUICheckbox()
	{
		checkboxBackground->destroy();
		checkboxFilling->destroy();
	}
	void GUICheckbox::inputUpdate()
	{
		GUIRectangle::inputUpdate();

		//Check mouse press
		if (getInputEnabled() && getMouseHover() && inputManager->isKeyPressed(MOUSE_BUTTON_LEFT))
			editorValue = !editorValue;

		ValueEditor::update();
	}
	void GUICheckbox::onEditorValueChange()
	{
		//Filling color
		if (editorValue)
			checkboxFilling->setColorAlpha(SELECTED_ALPHA);
		else
			checkboxFilling->setColorAlpha(UNSELECTED_ALPHA);
	}
	void GUICheckbox::setRenderState(const bool _state)
	{
		GUIRectangle::setRenderState(_state);
		checkboxFilling->setRenderState(_state && getRenderState());
		checkboxBackground->setRenderState(_state && getRenderState());
	}
	void GUICheckbox::setDepth(const int16_t depth)
	{
		GUIRectangle::setDepth(depth);
		checkboxFilling->setPlaneDepth(depth + 2);
		checkboxBackground->setPlaneDepth(depth + 1);
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
	void GUICheckbox::updateScale()
	{
		GUIRectangle::updateScale();

		checkboxBackground->resize(checkboxSize, checkboxSize);
		checkboxFilling->resize(checkboxSize - 2 * CHECKBOX_BORDER, checkboxSize - 2 * CHECKBOX_BORDER);
	}
	void GUICheckbox::updatePosition()
	{
		//Checkbox has to exceptionally scale first before positioning
		updateScale();

		GUIRectangle::updatePosition();

		checkboxBackground->setPosition(getXGlobal() + size.x - CHECKBOX_BORDER - checkboxSize, getYGlobal() + (size.y - checkboxSize) * 0.5f);
		checkboxFilling->setPosition(getXGlobal() + size.x - checkboxSize, getYGlobal() + (size.y - checkboxSize) * 0.5f + CHECKBOX_BORDER);

		//Update minimum size
		updateMinSize();
		
		//Text position
		if (text)
		{
			float textX = getXGlobal();
			if (checkBit(state, GUIRECT_TEXT_JUSTIFICATION_LEFT_BIT))
				textX += CHECKBOX_BORDER;
			else if (checkBit(state, GUIRECT_TEXT_JUSTIFICATION_RIGHT_BIT))
				textX += size.x - size.y - CHECKBOX_BORDER - text->getTextWidth() - borderWidth;
			else
				textX += 0.5f *(size.x - size.y - CHECKBOX_BORDER - text->getTextWidth());
			text->setPosition(std::round(textX), std::round(getYGlobal() + 0.5f * size.y - 0.5f * text->getTextHeight() - text->getFontDescender()));
		}
	}
}