#include "stdafx.h"
#include <algorithm>
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/GUI/GUIPopup.h"
#define BORDER_WIDTH 2
#define BACKGROUND_INDEX 0
#define MESSAGE_INDEX 1
#define OPTION1_INDEX 2



namespace se
{
	GUIPopup::GUIPopup(GUIContext& context, const std::string& _message)
		: GUIRectangleContainer(context)
		, escapeEnabled(true)
	{
		//Background rect
		addElement(new GUIRectangle(context));
		elements[BACKGROUND_INDEX]->disableState(GUIRECT_HOVER_COLOR_BIT);
		elements[BACKGROUND_INDEX]->setColor(se::Color(30, 35, 37, 200));

		//Message rect
		addElement(new GUIRectangle(context));
		back()->setString(_message);
		elements[MESSAGE_INDEX]->disableState(GUIRECT_HOVER_COLOR_BIT);
		elements[MESSAGE_INDEX]->setJustification(GUIRECT_TEXT_JUSTIFICATION_CENTER_BIT);
	}

	GUIPopup::~GUIPopup()
	{
	}

	void GUIPopup::addOptions(const Option& option)
	{
		options.push_back(option);
		addElement(new GUIRectangle(getGUIContext()));
		back()->setString(option.string);
		elements.back()->setJustification(GUIRECT_TEXT_JUSTIFICATION_CENTER_BIT);
	}

	void GUIPopup::inputUpdate()
	{
		GUIRectangleContainer::inputUpdate();
		if (checkState(GUIRECT_REMOVE_BIT))
			log::warning("GUIRECT_REMOVE_BIT is enabled!\nPopup should be removed by a higher authority!");
		else if (getInputManager().isKeyPressed(MOUSE_BUTTON_LEFT))
		{
			for (unsigned i = OPTION1_INDEX; i < elementsSize(); i++)
			{
				if (elements[i]->getMouseHover() && getInputManager().tryClaimMouseAvailability())
				{
					//Make callback?
					if (options[i - OPTION1_INDEX].hasPressCallback)
						options[i - OPTION1_INDEX].callback();
					enableState(GUIRECT_REMOVE_BIT);
				}
			}
		}
		if (getInputManager().isKeyPressed(KEYBOARD_ESCAPE) && escapeEnabled)
			enableState(GUIRECT_REMOVE_BIT);
	}

	void GUIPopup::updateMinSize()
	{
		GUIRectangle::updateMinSize();

		int optionsRowWidth(0);
		int optionsRowHeight(0);
		for (unsigned i = OPTION1_INDEX; i < elements.size(); i++)
		{
			optionsRowWidth += elements[i]->getMinWidth();
			if (elements[i]->getMinHeight() + BORDER_WIDTH > optionsRowHeight)
				optionsRowHeight = elements[i]->getMinHeight() + BORDER_WIDTH;
		}

		minSize.x = std::max(elements[MESSAGE_INDEX]->getMinWidth(), optionsRowWidth + BORDER_WIDTH * ((int)elements.size() - OPTION1_INDEX - 1));
		minSize.y = elements[MESSAGE_INDEX]->getMinHeight() + optionsRowHeight;

		if (minSize.x > size.x)
			setWidth(minSize.x);
		if (minSize.y > size.y)
			setWidth(minSize.y);
	}

	void GUIPopup::updateScale()
	{
		GUIRectangle::updateScale();

		int optionHeight(0);
		for (unsigned i = OPTION1_INDEX; i < elements.size(); i++)
		{
			if (elements[i]->getMinHeight() > optionHeight)
				optionHeight = elements[i]->getMinHeight();
		}

		elements[MESSAGE_INDEX]->setSize(size.x, optionHeight + BORDER_WIDTH);
		elements[BACKGROUND_INDEX]->setSize(size.x + 2 * BORDER_WIDTH, size.y + 2 * BORDER_WIDTH);

		int optionsWidth(-BORDER_WIDTH);
		for (unsigned i = OPTION1_INDEX; i < elements.size(); i++)
			optionsWidth += BORDER_WIDTH + elements[i]->getMinWidth();
		if (optionsWidth == minSize.x)
		{//Options row defines min width
			for (unsigned i = OPTION1_INDEX; i < elements.size(); i++)
				elements[i]->setSize(int(std::floor(float(size.x) / float(minSize.x) * float(elements[i]->getMinWidth()))), optionHeight);
		}
		else
		{//Message rect defines min width
			for (unsigned i = OPTION1_INDEX; i < elements.size(); i++)
				elements[i]->setSize(int(std::floor(float(elements[i]->getMinWidth()) / float(optionsWidth) * float(size.x))), optionHeight);
		}
	}

	void GUIPopup::updatePosition()
	{
		GUIRectangle::updatePosition();

		elements[BACKGROUND_INDEX]->setPositionLocal(-BORDER_WIDTH, -BORDER_WIDTH);
		if (elements.size() <= OPTION1_INDEX)
		{
			elements[MESSAGE_INDEX]->setPositionLocal(0, 0);
			return;
		}

		int x(0);
		for (unsigned i = OPTION1_INDEX; i < elements.size(); i++)
		{
			elements[i]->setPositionLocal(x, 0);
			x += elements[i]->getWidth() + BORDER_WIDTH;
		}
		elements[MESSAGE_INDEX]->setPositionLocal(0, elements[OPTION1_INDEX]->getHeight() + BORDER_WIDTH);
	}

	void GUIPopup::setBackgroundColor(const se::Color& rgb)
	{
		elements[BACKGROUND_INDEX]->setColor(rgb);
	}
}