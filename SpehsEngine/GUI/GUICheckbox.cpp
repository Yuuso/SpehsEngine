#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/Rendering/Polygon.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/GUI/GUICheckbox.h"
#include "SpehsEngine/Rendering/Text.h"


#define CHECKBOX_BORDER 2
#define SELECTED_ALPHA 255
#define UNSELECTED_ALPHA 25

namespace spehs
{
	const spehs::Color GUICheckbox::defaultCheckboxFillingColor(30, 30, 30);
	const spehs::Color GUICheckbox::defaultCheckboxOuterColor(160, 170, 180);

	GUICheckbox::GUICheckbox(GUIContext& context)
		: GUIRectangle(context)
		, ValueEditor(false)
		, checkboxSize(20)
	{
		checkboxBackground = getBatchManager().createPolygon(Shape::BUTTON, getDepth() + 1, 1.0f, 1.0f);
		checkboxBackground->setColor(defaultCheckboxFillingColor);
		checkboxBackground->setCameraMatrixState(false);

		checkboxFilling = getBatchManager().createPolygon(Shape::BUTTON, getDepth() + 2, 1.0f, 1.0f);
		checkboxFilling->setColor(defaultCheckboxOuterColor);
		checkboxFilling->setCameraMatrixState(false);

		onEditorValueChange();
		setJustification(GUIRECT_TEXT_JUSTIFICATION_LEFT_BIT);
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
		if (getInputEnabled() && getMouseHover() && getInputManager().isKeyPressed(MOUSE_BUTTON_LEFT))
			editorValue = !editorValue;

		ValueEditor::update();
	}

	void GUICheckbox::onEditorValueChange()
	{
		//Filling color
		if (editorValue)
			checkboxFilling->setAlpha(SELECTED_ALPHA);
		else
			checkboxFilling->setAlpha(UNSELECTED_ALPHA);
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
			minSize.x += (int)text->getTextWidth();
		minSize.y = 2 * CHECKBOX_BORDER + checkboxSize;
		if (text && text->getTextHeight() > (float)minSize.y)
			minSize.y = (int)text->getTextHeight();
	}

	void GUICheckbox::updateScale()
	{
		GUIRectangle::updateScale();

		checkboxBackground->resize((float)checkboxSize, (float)checkboxSize);
		checkboxFilling->resize(float(checkboxSize - 2 * CHECKBOX_BORDER), float(checkboxSize - 2 * CHECKBOX_BORDER));
	}

	void GUICheckbox::updatePosition()
	{
		//Checkbox has to exceptionally scale first before positioning
		updateScale();

		GUIRectangle::updatePosition();

		checkboxBackground->setPosition(float(getXGlobal() + size.x - CHECKBOX_BORDER - checkboxSize), float(getYGlobal() + (size.y - checkboxSize) * 0.5f));
		checkboxFilling->setPosition(float(getXGlobal() + size.x - checkboxSize), (float)getYGlobal() + (size.y - checkboxSize) * 0.5f + float(CHECKBOX_BORDER));

		//Update minimum size
		updateMinSize();
		
		//Text position
		if (text)
		{
			float textX = float(getXGlobal());
			if (checkBit(state, GUIRECT_TEXT_JUSTIFICATION_LEFT_BIT))
				textX += float(CHECKBOX_BORDER);
			else if (checkBit(state, GUIRECT_TEXT_JUSTIFICATION_RIGHT_BIT))
				textX += float(size.x - size.y - CHECKBOX_BORDER - text->getTextWidth() - borderWidth);
			else
				textX += 0.5f * (size.x - size.y - CHECKBOX_BORDER - text->getTextWidth());
			text->setPosition(std::round(textX), std::round(getYGlobal() + 0.5f * size.y - 0.5f * text->getTextHeight() - text->getFontDescender()));
		}
	}
}