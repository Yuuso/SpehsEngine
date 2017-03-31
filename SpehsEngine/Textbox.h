#pragma once
#include <string>
namespace spehs
{
	class Polygon;
	class Text;

	///A simple class for creating a text surrounded with a background polygon
	class Textbox
	{
	public:
		Textbox(const std::string& string, const unsigned textBorder, const int16_t depth);
		~Textbox();

		//Setters
		void setPosition(const int x, const int y);
		void setRenderState(const bool state);
		void setDepth(int16_t depth);
		void setString(const std::string& string);
		void setTextColor(const glm::vec3& rgb);
		void setTextColor(const glm::vec4& rgba);
		void setTextColor(const float r, const float g, const float b, const float a = 1.0f);
		void setTextColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a = 255);
		void setTextColorAlpha(const unsigned char a);
		void setTextColorAlpha(const float a);
		void setBackgroundColor(const glm::vec3& rgb);
		void setBackgroundColor(const glm::vec4& rgba);
		void setBackgroundColor(const float r, const float g, const float b, const float a = 1.0f);
		void setBackgroundColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a = 255);
		void setBackgroundColorAlpha(const unsigned char a);
		void setBackgroundColorAlpha(const float a);

		//Getters
		int getWidth() const;
		int getHeight() const;
		bool getRenderState() const;
		uint16_t getDepthBack() const;
		uint16_t getDepthFront() const;
		glm::vec2 getPosition() const;
		int getX() const;
		int getY() const;
		bool checkPointCollision(const int x, const int y) const;
		bool checkPointCollision(const glm::vec2& point) const;

	protected:
		spehs::Polygon* polygon;
		spehs::Text* text;
		const unsigned textBorder;
	};
}