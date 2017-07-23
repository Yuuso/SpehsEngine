#include <algorithm>
#include "StringOperations.h"
#include "GUIRectangle.h"
#include "Textbox.h"
#include "Polygon.h"
#include "Text.h"

namespace spehs
{
	Textbox::Textbox(const std::string& string, const unsigned _textBorder, const int16_t depth) : textBorder(_textBorder)
	{
		text = spehs::Text::create(string, GUIRectangle::defaultDepth);
		text->setFont("Fonts/Anonymous.ttf", 12);
		text->setCameraMatrixState(false);
		text->setColor(spehs::GUIRectangle::defaultTooltipStringColor);
		polygon = spehs::Polygon::create(spehs::Shape::BUTTON, GUIRectangle::defaultDepth + 1,
			2 * textBorder + std::max(text->getTextWidth(), (float)text->getFontMaxAdvanceWidth()),
			(1 + text->getLineCount()) * textBorder - text->getFontDescender() + text->getFontHeight() * text->getLineCount());
		polygon->setCameraMatrixState(false);
		polygon->setColor(spehs::GUIRectangle::defaultTooltipColor);
		setRenderState(true);
		setDepth(depth);
	}
	Textbox::~Textbox()
	{
		text->destroy();
		polygon->destroy();
	}
	void Textbox::setDepth(int16_t depth)
	{
		polygon->setPlaneDepth(depth);
		text->setPlaneDepth(depth + 1);
	}
	void Textbox::setPosition(const int x, const int y)
	{
		polygon->setPosition(x, y);
		text->setPosition(x + textBorder, y + textBorder - text->getFontDescender());
	}
	void Textbox::setRenderState(const bool state)
	{
		polygon->setRenderState(state);
		text->setRenderState(state);
	}
	void Textbox::setString(const std::string& string)
	{
		text->setString(string);
		polygon->resize(
			2 * textBorder + std::max(text->getTextWidth(), (float)text->getFontMaxAdvanceWidth()),
			(1 + text->getLineCount()) * textBorder - text->getFontDescender() + text->getFontHeight() * text->getLineCount());
		setPosition(polygon->getX(), polygon->getY());
	}
	void Textbox::setTextColor(const spehs::Color& color)
	{
		text->setColor(color);
	}
	void Textbox::setTextAlpha(const spehs::Color::Component& alpha)
	{
		text->setAlpha(alpha);
	}
	void Textbox::setBackgroundColor(const spehs::Color& color)
	{
		polygon->setColor(color);
	}
	void Textbox::setBackgroundAlpha(const spehs::Color::Component& alpha)
	{
		polygon->setAlpha(alpha);
	}


	//GETTERS
	uint16_t Textbox::getDepthBack() const
	{
		return polygon->getPlaneDepth();
	}
	uint16_t Textbox::getDepthFront() const
	{
		return text->getPlaneDepth();
	}
	int Textbox::getWidth() const
	{
		return polygon->getWidth();
	}
	int Textbox::getHeight() const
	{
		return polygon->getHeight();
	}
	bool Textbox::getRenderState() const
	{
		return text->getRenderState() && polygon->getRenderState();
	}
	spehs::vec2 Textbox::getPosition() const
	{
		return polygon->getPosition();
	}
	int Textbox::getX() const
	{
		return polygon->getX();
	}
	int Textbox::getY() const
	{
		return polygon->getY();
	}
	bool Textbox::checkPointCollision(const int x, const int y) const
	{
		return !(x < polygon->getX() || y < polygon->getY() || x > polygon->getX() + polygon->getWidth() || y > polygon->getY() + polygon->getHeight());
	}
	bool Textbox::checkPointCollision(const spehs::vec2& point) const
	{
		return !(point.x < polygon->getX() || point.y < polygon->getY() || point.x > polygon->getX() + polygon->getWidth() || point.y > polygon->getY() + polygon->getHeight());
	}

}