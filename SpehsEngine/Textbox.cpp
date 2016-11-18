#include "StringOperations.h"
#include "GUIRectangle.h"
#include "Textbox.h"
#include "Polygon.h"
#include "Text.h"
#define TEXT_BORDER 2

namespace spehs
{
	Textbox::Textbox(std::string& string)
	{
		text = spehs::Text::create(string, GUIRectangle::defaultDepth);
		text->setFont("Fonts/Anonymous.ttf", 12);
		text->setCameraMatrixState(false);
		text->setColor(spehs::GUIRectangle::defaultTooltipStringColor);
		polygon = spehs::Polygon::create(spehs::Shape::BUTTON, GUIRectangle::defaultDepth + 1, text->getTextWidth() + 2 * TEXT_BORDER, text->getTextHeight() + 2 * TEXT_BORDER);
		polygon->setCameraMatrixState(false);
		polygon->setColor(spehs::GUIRectangle::defaultTooltipColor);
		setRenderState(false);
	}
	Textbox::Textbox(std::string& string, uint16_t depth) : Textbox(string)
	{
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
	uint16_t Textbox::getDepthBack()
	{
		return polygon->getPlaneDepth();
	}
	uint16_t Textbox::getDepthFront()
	{
		return text->getPlaneDepth();
	}
	void Textbox::setPosition(const int x, const int y)
	{
		polygon->setPosition(x, y);
		text->setPosition(x + TEXT_BORDER, y + TEXT_BORDER);
	}
	void Textbox::setRenderState(const bool state)
	{
		polygon->setRenderState(state);
		text->setRenderState(state);
	}
	bool Textbox::checkPointCollision(const int x, const int y)
	{
		return !(x < polygon->getX() || y < polygon->getY() || x > polygon->getX() + polygon->getWidth() || y > polygon->getY() + polygon->getHeight());
	}
	bool Textbox::checkPointCollision(const glm::vec2& point)
	{
		return !(point.x < polygon->getX() || point.y < polygon->getY() || point.x > polygon->getX() + polygon->getWidth() || point.y > polygon->getY() + polygon->getHeight());
	}

}