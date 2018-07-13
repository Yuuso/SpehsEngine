#pragma once
#include <string>
#include "SpehsEngine/Core/Vector.h"
#include "SpehsEngine/Core/Color.h"

namespace se
{
	namespace rendering
	{
		class BatchManager;
		class Polygon;
		class Text;
	}

	///A simple class for creating a text surrounded with a background polygon
	class Textbox
	{
	public:
		Textbox(se::rendering::BatchManager& batchManager, const std::string& string, const unsigned textBorder, const int16_t depth);
		~Textbox();

		//Setters
		void setPosition(const int x, const int y);
		void setRenderState(const bool state);
		void setDepth(int16_t depth);
		void setString(const std::string& string);
		void setTextColor(const se::Color& color);
		void setTextAlpha(const float alpha);
		void setBackgroundColor(const se::Color& color);
		void setBackgroundAlpha(const float alpha);

		//Getters
		int getWidth() const;
		int getHeight() const;
		bool getRenderState() const;
		uint16_t getDepthBack() const;
		uint16_t getDepthFront() const;
		se::vec2 getPosition() const;
		int getX() const;
		int getY() const;
		bool checkPointCollision(const int x, const int y) const;
		bool checkPointCollision(const se::vec2& point) const;

	protected:
		se::rendering::Polygon* polygon;
		se::rendering::Text* text;
		const unsigned textBorder;
	};
}