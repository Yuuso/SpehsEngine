#include <algorithm>
#include "GUIPopup.h"

namespace spehs
{
	GUIPopup::GUIPopup(std::string _message)
	{
		addElement(new GUIRectangle(_message));
		elements.front()->setJustification(GUIRECT_TEXT_JUSTIFICATION_CENTER);
	}
	GUIPopup::~GUIPopup()
	{
	}
	void GUIPopup::addOptions(GUIPopupOption& option)
	{
		options.push_back(option);
		addElement(new GUIRectangle(option.string));
		elements.back()->setJustification(GUIRECT_TEXT_JUSTIFICATION_CENTER);

	}
	void GUIPopup::updatePosition()
	{
		GUIRectangle::updatePosition();

		if (elements.size() == 1)
		{
			elements.front()->setPositionLocal(0, elements.front()->getMinHeight());
			return;
		}

		elements.front()->setPositionLocal(0, elements[1]->getMinHeight());
		int x(0);
		for (unsigned i = 1; i < elements.size(); i++)
		{
			elements[i]->setPositionLocal(x, 0);
			x += elements[i]->getWidth();
		}
	}
	void GUIPopup::updateScale()
	{
		GUIRectangle::updateScale();

		int optionHeight(0);
		for (unsigned i = 1; i < elements.size(); i++)
		{
			if (elements[i]->getMinHeight() > optionHeight)
				optionHeight = elements[i]->getMinHeight();
		}

		elements.front()->setSize(size.x, size.y - optionHeight);
		for (unsigned i = 1; i < elements.size(); i++)
		{
			elements[i]->setSize(std::floor(size.x / float(minSize.x) * elements[i]->getMinWidth()), optionHeight);
		}
	}
	void GUIPopup::updateMinSize()
	{
		GUIRectangle::updateMinSize();

		int optionsRowWidth(0);
		int optionsRowHeight(0);
		for (unsigned i = 1; i < elements.size(); i++)
		{
			optionsRowWidth += elements[i]->getMinWidth();
			if (elements[i]->getMinHeight() > optionsRowHeight)
				optionsRowHeight = elements[i]->getMinHeight();
		}

		minSize.x = std::max(elements.front()->getMinWidth(), optionsRowWidth);
		minSize.y = elements.front()->getMinHeight() + optionsRowHeight;
	}
}