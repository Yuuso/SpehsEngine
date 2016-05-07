#include <iostream>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <Freetype/ft2build.h>
#include FT_FREETYPE_H

#include "Exceptions.h"
#include "Text.h"
#include "GLSLProgram.h"
#include "ApplicationData.h"
#include "OpenGLError.h"

namespace spehs
{
	struct Font
	{
		~Font();

		FT_Face* ftFace = nullptr;
		std::string fontPath;
		int fontSize;
		std::map<GLchar, Character> characters;
		int referenceCount = 0;
		int height = 0;
		int descender = 0;
	};

	static bool textRenderingInitialized = false;
	static int textCount = 0;
	static FT_Library* ft = nullptr;
	static GLSLProgram textProgram;
	static std::vector<Font*> fonts;
	static glm::mat4 projectionMatrix;

	void initializeTextRendering()
	{
		if (textRenderingInitialized)
		{
			exceptions::unexpectedError("Text rendering already initialized!");
			return;
		}

		if (ft != nullptr)
		{
			exceptions::unexpectedError("Freetype library already exists!");
			return;
		}
		ft = new FT_Library;
		if (FT_Init_FreeType(ft))
		{
			exceptions::fatalError("Freetype library initialization failed!");
			return;
		}

		textProgram.compileShaders("Shaders/text.vertex", "Shaders/text.fragment");
		textProgram.addAttribute("vertex");
		textProgram.linkShaders();

		textRenderingInitialized = true;
	}
	void uninitializeTextRendering()
	{
		if (!textRenderingInitialized)
		{//Validate uninitialization
			return;
		}

		//Uninitialize FreeType
		FT_Done_FreeType(*ft);
		delete ft;
		ft = nullptr;

		//Inform (possible) memory leaks
		if (textCount != 0)
		{
			std::cout << "\nSome Text objects were not deallocated! Remaining gines::Text count: " << textCount;
		}
		if (fonts.size() != 0)
		{
			std::cout << "\nSome Font objects were not deallocated! Remaining font count: " << fonts.size();
		}

		//Uninitialization complete
		std::cout << "\nText rendering uninitialized";
		textRenderingInitialized = false;
	}

	Font::~Font()
	{
		if (ftFace != nullptr)
		{
			FT_Done_Face(*ftFace);
			delete ftFace;
		}
	}

	Text::~Text()
	{
		glDeleteBuffers(1, &vertexArrayData);
		unreferenceFont();
		if (--textCount <= 0)
		{
			uninitializeTextRendering();
		}
	}
	Text::Text(PlaneDepth depth) : glyphsToRender(0), lineCount(0), vertexArrayData(0), textures(nullptr), scale(1.0f), lineSpacing(0), doUpdate(true), font(nullptr)
	{//Default constructor is called from copy constructor as well
		textCount++;
	}
	Text::Text(const Text& original)
	{//Copy constructor
		*this = original;
	}
	void Text::operator=(const Text& original)
	{
		glDeleteBuffers(1, &vertexArrayData);
		delete[] textures;
		glGenBuffers(1, &vertexArrayData);
		string = original.string;
		position = original.position;
		color = original.color;
		updateGlyphsToRender();
		textures = nullptr;
		scale = original.scale;
		lineSpacing = original.lineSpacing;
		lineCount = original.lineCount;

		//Font
		unreferenceFont();
		if (original.font != nullptr)
		{
			setFont(original.font->fontPath, original.font->fontSize);//Increases reference count
			updateBuffers();
		}
	}
	void Text::unreferenceFont()
	{
		if (font == nullptr)
		{
			return;
		}

		//if face becomes useless, remove
		if (--font->referenceCount <= 0)
		{
			for (unsigned int i = 0; i < fonts.size(); i++)
			if (fonts[i] == font)
			{
				fonts.erase(fonts.begin() + i);
				delete font;
			}
		}
		font = nullptr;
	}
	bool Text::setFontSize(int size)
	{
		//No font loaded
		if (font == nullptr)
		{
			return false;
		}

		//The size already matches
		if (font->fontSize == size)
		{
			return true;
		}

		//Get new font face
		setFont(font->fontPath, size);
		return true;
	}
	bool Text::setFont(std::string fontPath, int size)
	{
		
		if (!textRenderingInitialized)
		{//make sure text is initialized
			initializeTextRendering();
		}

		if (font != nullptr)
		{//If a font is active, unreference it
			unreferenceFont();
		}

		for (unsigned int i = 0; i < fonts.size(); i++)
		{//Check whether the font that is being looked for exists...
			if (fontPath == fonts[i]->fontPath)
			{//... with the correct size
				if (size == fonts[i]->fontSize)
				{//Font already loaded
					font = fonts[i];
					font->referenceCount++;
					doUpdate = true;
					return true;
				}
			}
		}

		//Create a new font
		fonts.push_back(new Font);
		font = fonts.back();
		font->ftFace = new FT_Face;

		FT_Error error = FT_New_Face(*ft, fontPath.c_str(), 0, font->ftFace);
		if (error)
		{
			//std::string errorString = "Freetype error: Failed to load font "; errorString += fontPath; errorString += " code: " + error;
			//logError(errorString);
			return false;
		}

		font->fontPath = fontPath;
		font->fontSize = size;
		font->referenceCount = 1;
		FT_Face* ftFace = font->ftFace;
		FT_Set_Pixel_Sizes(*ftFace, 0, size);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

		for (GLubyte c = 32; c <= 126; c++)
		{
			// Load character glyph
			if (FT_Load_Char(*ftFace, c, FT_LOAD_RENDER))
			{
				//logError("FreeType error: Failed to load Glyph");
				return false;
			}

			// Generate texture
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				(*ftFace)->glyph->bitmap.width,
				(*ftFace)->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				(*ftFace)->glyph->bitmap.buffer
				);

			// Set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// Now store character for later use
			Character character =
			{
				texture,
				glm::ivec2((*ftFace)->glyph->bitmap.width, (*ftFace)->glyph->bitmap.rows),
				glm::ivec2((*ftFace)->glyph->bitmap_left, (*ftFace)->glyph->bitmap_top),
				(*ftFace)->glyph->advance.x
			};
			font->characters.insert(std::pair<GLchar, Character>(c, character));
		}

		checkOpenGLErrors(__FILE__, __LINE__);

		font->height = (*font->ftFace)->size->metrics.height >> 6;
		font->descender = (*font->ftFace)->descender >> 6;
		projectionMatrix = glm::ortho(0.0f, float(applicationData->getWindowWidth()), 0.0f, float(applicationData->getWindowHeight()));

		doUpdate = true;

		return true;
	}
	void Text::updateBuffers()
	{
		if (!textRenderingInitialized)
		{
			return;
		}

		//This function should be called everytime the number of glyphs to render changes
		updateGlyphsToRender();
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glDeleteBuffers(1, &vertexArrayData);
		glGenBuffers(1, &vertexArrayData);
		glBindBuffer(GL_ARRAY_BUFFER, vertexArrayData);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4 * glyphsToRender, NULL, GL_DYNAMIC_DRAW);
		// The 2D quad requires 6 vertices of 4 floats each so we reserve 6 * 4 floats of memory.
		// Because we'll be updating the content of the VBO's memory quite often we'll allocate the memory with GL_DYNAMIC_DRAW.

		int x = position.x;
		int y = position.y;

		// Iterate through all characters
		if (textures != nullptr)
		{
			delete[] textures;
		}
		textures = new GLuint[glyphsToRender];
		GLfloat* vertices = new GLfloat[24 * glyphsToRender];
		int _index = 0;
		for (auto c = string.begin(); c != string.end(); c++)
			if (*c != '\n')
			{
			Character ch = font->characters[*c];

			GLfloat xpos = x + ch.bearing.x * scale;
			GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

			GLfloat w = ch.size.x * scale;
			GLfloat h = ch.size.y * scale;

			// Update VBO for each character
			vertices[_index * 24 + 0] = xpos;
			vertices[_index * 24 + 1] = ypos + h;
			vertices[_index * 24 + 2] = 0.0f;
			vertices[_index * 24 + 3] = 0.0f;

			vertices[_index * 24 + 4] = xpos;
			vertices[_index * 24 + 5] = ypos;
			vertices[_index * 24 + 6] = 0.0f;
			vertices[_index * 24 + 7] = 1.0f;

			vertices[_index * 24 + 8] = xpos + w;
			vertices[_index * 24 + 9] = ypos;
			vertices[_index * 24 + 10] = 1.0f;
			vertices[_index * 24 + 11] = 1.0f;

			vertices[_index * 24 + 12] = xpos;
			vertices[_index * 24 + 13] = ypos + h;
			vertices[_index * 24 + 14] = 0.0f;
			vertices[_index * 24 + 15] = 0.0f;

			vertices[_index * 24 + 16] = xpos + w;
			vertices[_index * 24 + 17] = ypos;
			vertices[_index * 24 + 18] = 1.0f;
			vertices[_index * 24 + 19] = 1.0f;

			vertices[_index * 24 + 20] = xpos + w;
			vertices[_index * 24 + 21] = ypos + h;
			vertices[_index * 24 + 22] = 1.0f;
			vertices[_index * 24 + 23] = 0.0f;

			// Render glyph texture over quad
			textures[_index] = ch.textureID;

			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
			_index++;
			}
			else
			{//new line
				x = position.x;
				y -= font->height + lineSpacing;
			}

		checkOpenGLErrors(__FILE__, __LINE__);

		//Submit data
		glBindBuffer(GL_ARRAY_BUFFER, vertexArrayData);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 24 * glyphsToRender, vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		delete[] vertices;
		doUpdate = false;

		checkOpenGLErrors(__FILE__, __LINE__);
	}
	void Text::updateGlyphsToRender()
	{
		glyphsToRender = 0;
		for (auto c = string.begin(); c != string.end(); c++)
			if (*c != '\n')
			{
			glyphsToRender++;
			}
	}

	void Text::setRenderState(bool state)
	{
		if (doUpdate)
			updateBuffers();

		//Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		textProgram.use();

		glBindBuffer(GL_ARRAY_BUFFER, vertexArrayData);
		glUniformMatrix4fv(textProgram.getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glUniform4f(textProgram.getUniformLocation("textColor"), color.r, color.g, color.b, color.a);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glActiveTexture(GL_TEXTURE0);
		for (int i = 0; i < glyphsToRender; i++)
		{//Draw
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glDrawArrays(GL_TRIANGLES, i * 6, 6);
		}

		//Unbinds / unuse program
		glDisableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		textProgram.unuse();

		checkOpenGLErrors(__FILE__, __LINE__);
	}

	void Text::setString(std::string str)
	{
		//Set line count
		if (str.size() > 0) lineCount = 1; else lineCount = 0;
		for (unsigned i = 0; i < str.size(); i++)
		if (str[i] == '\n') lineCount++;

		string = str;
		doUpdate = true;
	}
	void Text::setString(char* str, unsigned length)
	{
		//Set line count
		if (length > 0) lineCount = 1; else lineCount = 0;
		for (unsigned i = 0; i < length; i++)
			if (str[i] == '\n') lineCount++;

		string = str;
		doUpdate = true;
	}
	void Text::incrementString(std::string str)
	{
		//Increase line count
		for (unsigned i = 0; i < str.size(); i++)
		if (str[i] == '\n') lineCount++;

		string += str;
		doUpdate = true;
	}
	void Text::incrementFrontString(std::string str)
	{
		//Increase line count
		for (unsigned i = 0; i < str.size(); i++)
		if (str[i] == '\n') lineCount++;

		string = str + string;
		doUpdate = true;
	}
	void Text::setPosition(glm::vec2& vec)
	{
		position.x = vec.x;
		position.y = vec.y;
		doUpdate = true;
	}
	void Text::setPosition(float _x, float _y)
	{
		position.x = _x;
		position.y = _y;
		doUpdate = true;
	}
	void Text::translate(glm::vec2& vec)
	{
		position.x += vec.x;
		position.y += vec.y;
		doUpdate = true;
	}
	void Text::setColor(glm::vec4& vec)
	{
		color = vec;
	}
	void Text::setColor(float r, float g, float b, float a)
	{
		color.r = r;
		color.g = g;
		color.b = b;
		color.a = a;
	}
	void Text::setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		color.r = r / 255.0f;
		color.g = g / 255.0f;
		color.b = b / 255.0f;
		color.a = a / 255.0f;
	}
	void Text::setAlpha(float alpha)
	{
		color.a = alpha;
	}
	void Text::setAlpha(unsigned char a)
	{
		color.a = a / 255.0f;
	}
	int Text::getLineCount()
	{
		return lineCount;
	}
	int Text::getFontHeight()
	{
		return font->height;
	}
	glm::vec4& Text::getColorRef()
	{
		return color;
	}
	int Text::getGlyphsToRender()
	{
		return glyphsToRender;
	}
	std::string Text::getString()
	{
		return string;
	}
	int Text::getTextWidth()
	{
		int record = 0;
		int currentLineWidth = 0;

		for (unsigned i = 0; i < string.size(); i++)
		if (string[i] == '\n')
		{//New line incoming, check current line width and compare with record. Reset current line width.
			if (currentLineWidth > record)
				record = currentLineWidth;
			currentLineWidth = 0;
		}
		else
		{//Increase current line width
			currentLineWidth += font->characters[string[i]].advance;
		}
		
		if (currentLineWidth > record)
			return currentLineWidth >> 6;
		return record >> 6;
	}
	int Text::getTextHeight()
	{
		int w = getFontHeight() * lineCount;
		if (lineCount > 1)
			w += (lineCount - 1) * lineSpacing;

		return w;
	}
	int Text::getFontDescender()
	{ 
		return font->descender; 
	}
	Font* Text::getFontPtr()
	{
		return font; 
	}
	int Text::getFontSize()
	{
		return font->fontSize; 
	}
}