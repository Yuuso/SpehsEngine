#pragma once
#include <string>
#include "Color.h"
#include "Vector.h"

namespace spehs
{
	class BatchManager;
	class Polygon;
	class Text;

	///A simple class for creating a text surrounded with a background polygon
	class Textbox
	{
	public:
		Textbox(BatchManager& batchManager, const std::string& string, const unsigned textBorder, const int16_t depth);
		~Textbox();

		//Setters
		void setPosition(const int x, const int y);
		void setRenderState(const bool state);
		void setDepth(int16_t depth);
		void setString(const std::string& string);
		void setTextColor(const spehs::Color& color);
		void setTextAlpha(const spehs::Color::Component& alpha);
		void setBackgroundColor(const spehs::Color& color);
		void setBackgroundAlpha(const spehs::Color::Component& alpha);

		//Getters
		int getWidth() const;
		int getHeight() const;
		bool getRenderState() const;
		uint16_t getDepthBack() const;
		uint16_t getDepthFront() const;
		spehs::vec2 getPosition() const;
		int getX() const;
		int getY() const;
		bool checkPointCollision(const int x, const int y) const;
		bool checkPointCollision(const spehs::vec2& point) const;

	protected:
		spehs::Polygon* polygon;
		spehs::Text* text;
		const unsigned textBorder;
	};
}