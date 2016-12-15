#pragma once
#include <string>
namespace spehs
{
	class Polygon;
	class Text;

	///A class for simply a box filled with static text. Uses GUI color settings as default. Polygon and text members visible for public
	struct Textbox
	{
		Textbox(std::string& string);
		Textbox(std::string& string, int16_t depth);
		~Textbox();

		virtual bool checkPointCollision(const int x, const int y);
		virtual bool checkPointCollision(const glm::vec2& point);
		virtual void setPosition(const int x, const int y);
		virtual void setRenderState(const bool state);
		virtual void setDepth(int16_t depth);
		virtual uint16_t getDepthBack();
		virtual uint16_t getDepthFront();

		spehs::Polygon* polygon;
		spehs::Text* text;
	};
}