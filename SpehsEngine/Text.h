#pragma once
#include <map>
#include <string>
#include <glm/glm.hpp>


typedef unsigned int GLuint;
typedef char GLchar;
typedef int16_t PlaneDepth;

namespace spehs
{
	void initializeTextRendering();
	void uninitializeTextRendering();
	struct Character
	{
		GLuint     textureID;  // ID handle of the glyph texture
		glm::ivec2 size;       // Size of glyph
		glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
		GLuint     advance;    // Offset to advance to next glyph
	};
	struct Font;
	class Text
	{
	public:
		Text(PlaneDepth depth);
		Text(const Text& original);
		~Text();
		void operator=(const Text& original);

		bool setFont(std::string fontPath, int size);
		bool setFontSize(int size);
		void setRenderState(bool state);
		void setString(std::string str);
		void setString(char* str, unsigned length);
		void incrementString(std::string str);
		void incrementFrontString(std::string str);
		void setColor(glm::vec4& col);
		/*Set color in floating range [0.0f - 1.0f]*/
		void setColor(float r, float g, float b, float a = 1.0f);
		void setPosition(glm::vec2& vec);
		void setPosition(float, float);
		void setPlaneDepth(PlaneDepth depth){/*TODO*/ }
		void translate(glm::vec2& vec);

		void updateGlyphsToRender();
		void updateBuffers();

		//Getters
		int getFontHeight();
		int getLineCount();
		glm::vec4& getColorRef();
		int getGlyphsToRender();
		std::string getString();
		int getTextWidth();
		int getTextHeight();
		int getFontDescender();
		Font* getFontPtr();
		int getFontSize();

	private:
		int glyphsToRender;
		int lineCount;
		GLuint vertexArrayData;
		GLuint* textures;
		glm::vec2 position;
		glm::vec4 color;
		float scale;
		int lineSpacing;
		bool doUpdate;
		std::string string;

		/*If text has an font assigned, reassign it and reduce font's reference count by 1*/
		void unreferenceFont();
		Font* font;
	};
}