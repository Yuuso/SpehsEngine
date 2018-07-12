#include "stdafx.h"
#include <algorithm>
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/GUI/GUIRectangle.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/GUI/Textbox.h"
#include "SpehsEngine/Rendering/Polygon.h"
#include "SpehsEngine/Rendering/Text.h"
#include "SpehsEngine/Core/Shapes.h"

namespace se
{
	Textbox::Textbox(BatchManager& batchManager, const std::string& string, const unsigned _textBorder, const int16_t depth)
		: textBorder(_textBorder)
	{
		text = batchManager.createText(string, GUIRectangle::defaultDepth);
		text->setFont("Fonts/Anonymous.ttf", 12);
		text->setCameraMatrixState(false);
		text->setColor(se::GUIRectangle::defaultTooltipStringColor);
		polygon = batchManager.createPolygon(se::Shape::BUTTON, GUIRectangle::defaultDepth + 1,
			float(2 * textBorder) + std::max(text->getTextWidth(), (float)text->getFontMaxAdvanceWidth()),
			float((1 + text->getLineCount()) * textBorder - text->getFontDescender() + text->getFontHeight() * text->getLineCount()));
		polygon->setCameraMatrixState(false);
		polygon->setColor(se::GUIRectangle::defaultTooltipColor);
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
		polygon->setPosition(float(x), float(y));
		text->setPosition(float(x + textBorder), float(y + textBorder - text->getFontDescender()));
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
			float(2 * textBorder) + std::max(text->getTextWidth(), (float)text->getFontMaxAdvanceWidth()),
			float((1 + text->getLineCount()) * textBorder - text->getFontDescender() + text->getFontHeight() * text->getLineCount()));
		setPosition(int(polygon->getX()), int(polygon->getY()));
	}
	void Textbox::setTextColor(const se::Color& color)
	{
		text->setColor(color);
	}
	void Textbox::setTextAlpha(const float alpha)
	{
		text->setAlpha(alpha);
	}
	void Textbox::setBackgroundColor(const se::Color& color)
	{
		polygon->setColor(color);
	}
	void Textbox::setBackgroundAlpha(const float alpha)
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
		return (int)polygon->getWidth();
	}
	int Textbox::getHeight() const
	{
		return (int)polygon->getHeight();
	}
	bool Textbox::getRenderState() const
	{
		return text->getRenderState() && polygon->getRenderState();
	}
	se::vec2 Textbox::getPosition() const
	{
		return polygon->getPosition();
	}
	int Textbox::getX() const
	{
		return (int)polygon->getX();
	}
	int Textbox::getY() const
	{
		return (int)polygon->getY();
	}
	bool Textbox::checkPointCollision(const int x, const int y) const
	{
		return !(x < polygon->getX() || y < polygon->getY() || x > polygon->getX() + polygon->getWidth() || y > polygon->getY() + polygon->getHeight());
	}
	bool Textbox::checkPointCollision(const se::vec2& point) const
	{
		return !(point.x < polygon->getX() || point.y < polygon->getY() || point.x > polygon->getX() + polygon->getWidth() || point.y > polygon->getY() + polygon->getHeight());
	}

}