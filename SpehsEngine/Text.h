#pragma once
#include <map>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H




namespace SpehsEngine
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
	struct Font
	{
		~Font()
		{
			if (ftFace != nullptr)
			{
				FT_Done_Face(*ftFace);
				delete ftFace;
			}
		}

		FT_Face* ftFace = nullptr;
		char* fontPath;
		int fontSize;
		std::map<GLchar, Character> characters;
		int referenceCount = 0;
		int height = 0;
		int descender = 0;
	};

	class Text
	{
	public:
		Text();
		Text(const Text& original);
		~Text();
		void operator=(const Text& original);

		bool setFont(char* fontPath, int size);
		bool setFontSize(int size);
		void render();
		void setString(std::string str);
		void incrementString(std::string str);
		void incrementFrontString(std::string str);
		void setColor(glm::vec4& col);
		/*Set color in floating range [0.0f - 1.0f]*/
		void setColor(float r, float g, float b, float a = 1.0f);
		void setPosition(glm::vec2& vec);
		void setPosition(float, float);
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
		int getFontDescender(){ return font->descender; }
		Font* getFontPtr(){ return font; }
		int getFontSize(){ return font->fontSize; }

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