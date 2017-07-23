#include <algorithm>
#include "ApplicationData.h"
#include "ShaderManager.h"
#include "BatchManager.h"
#include "GLSLProgram.h"
#include "OpenGLError.h"
#include "Exceptions.h"
#include "Text.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <Freetype/ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <atomic>

std::atomic<int> textAllocations;
std::atomic<int> textDeallocations;


namespace spehs
{
	static bool textRenderingInitialized(false);
	static FT_Library* ft = nullptr;

	void initText()
	{
		FontManager::instance = new FontManager;

		if (textRenderingInitialized)
		{
			exceptions::unexpectedError("Text rendering already initialized!");
			return;
		}

		if (ft)
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

		textRenderingInitialized = true;
	}
	void uninitText()
	{
		delete FontManager::instance;

		if (!textRenderingInitialized)
		{//Validate uninitialization
			exceptions::unexpectedError("Text rendering already uninitialized");
			return;
		}

		//Uninitialize FreeType
		FT_Done_FreeType(*ft);
		delete ft;
		ft = nullptr;

		//Uninitialization complete
		std::cout << "\nText rendering uninitialized.";
		textRenderingInitialized = false;
	}


	struct Font
	{
		~Font()
		{
			if (ftFace != nullptr)
			{
				FT_Error error = FT_Done_Face(*ftFace);
				if (error)
				{
					std::string errorString = "Freetype error: Failed to unload font ";
					errorString += fontPath;
					errorString += " code: " + error;
					exceptions::unexpectedError(errorString);
				}
				delete ftFace;

				for (unsigned i = 0; i < characters.size(); i++)
				{
					glDeleteTextures(1, &characters[i].textureID);
				}
			}
		}

		FT_Face* ftFace = nullptr;
		std::string fontPath;
		int fontSize;
		std::map<GLchar, Character> characters;
		int referenceCount = 0;
		int height = 0;
		int ascender = 0;
		int descender = 0;
		int maxAdvanceWidth = 0;
	};


	FontManager* FontManager::instance = nullptr;
	FontManager::FontManager()
	{
	}
	FontManager::~FontManager()
	{
		//Inform (possible) memory leaks
		if (fonts.size() != 0)
		{
			std::cout << "\nSome Font objects were not deallocated! Remaining font count: " << fonts.size();
			for (unsigned i = 0; i < fonts.size(); i++)
			{
				delete fonts[i];
			}
		}
	}
	Font* FontManager::getFont(const std::string &_fontPath, const int &_size)
	{
		//Check for already existing font
		for (unsigned int i = 0; i < fonts.size(); i++)
		{
			if (_fontPath == fonts[i]->fontPath)
			{
				if (_size == fonts[i]->fontSize)
				{
					fonts[i]->referenceCount++;
					return fonts[i];
				}
			}
		}

		//Create a new font
		fonts.push_back(new Font);
		Font* font = fonts.back();
		font->ftFace = new FT_Face;

		FT_Error error = FT_New_Face(*ft, _fontPath.c_str(), 0, font->ftFace);
		if (error)
		{			
			std::string errorString = "Freetype error: Failed to load font "; 
			errorString += _fontPath; 
			errorString += " code: " + error;
			exceptions::fatalError(errorString);
			return nullptr;
		}

		font->fontPath = _fontPath;
		font->fontSize = _size;
		font->referenceCount = 1;

		FT_Face* ftFace = font->ftFace;
		FT_Set_Pixel_Sizes(*ftFace, 0, _size);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //Disable byte-alignment restriction

		for (GLubyte c = 32; c <= 126; c++)
		{
			//Load character glyph
			if (FT_Load_Char(*ftFace, c, FT_LOAD_RENDER))
			{
				exceptions::unexpectedError("FreeType error: Failed to load Glyph");
				return nullptr;
			}

			//Generate texture
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

			//Set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			//Now store character for later use
			Character character =
			{
				texture,
				spehs::ivec2((*ftFace)->glyph->bitmap.width, (*ftFace)->glyph->bitmap.rows),
				spehs::ivec2((*ftFace)->glyph->bitmap_left, (*ftFace)->glyph->bitmap_top),
				(*ftFace)->glyph->advance.x
			};
			font->characters.insert(std::pair<GLchar, Character>(c, character));
		}

		checkOpenGLErrors(__FILE__, __LINE__);

		font->height = (*font->ftFace)->size->metrics.height >> 6;
		font->ascender = (*font->ftFace)->ascender >> 6;
		font->descender = (*font->ftFace)->descender >> 6;
		font->maxAdvanceWidth = (*font->ftFace)->max_advance_width >> 6;

		return font;
	}
	void FontManager::unreferenceFont(Font* _font)
	{
		if (_font == nullptr)
			return;

		//If face becomes useless, remove it
		if (--_font->referenceCount <= 0)
		{
			for (unsigned int i = 0; i < fonts.size(); i++)
			{
				if (fonts[i] == _font)
				{
					fonts.erase(fonts.begin() + i);
					delete _font;
				}
			}
		}
		_font = nullptr;
	}
	

	Text* Text::create(const PlaneDepth &_depth)
	{
		return getActiveBatchManager()->createText(_depth);
	}
	Text* Text::create(const std::string &_string, const PlaneDepth &_depth)
	{
		return getActiveBatchManager()->createText(_string, _depth);
	}
	Text::~Text()
	{
		FontManager::instance->unreferenceFont(font);
#ifdef _DEBUG
		textDeallocations++;
#endif
	}
	Text::Text(const PlaneDepth depth) : lineCount(0), scale(1.0f), lineSpacing(0), font(nullptr), needTextUpdate(false), needPositionUpdate(false), renderState(true), readyForDelete(false),
		shaderIndex(DefaultText), planeDepth(depth), cameraMatrixState(false)
	{
#ifdef _DEBUG
		textAllocations++;
#endif
	}
	Text::Text(const std::string &_string, const PlaneDepth _depth) : Text(_depth)
	{
		setString(_string);
	}
	/*
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
	}*/

	void Text::destroy()
	{
		readyForDelete = true;
	}

	void Text::update()
	{
		if (needTextUpdate)
		{
			updateText();
		}
		if (needPositionUpdate)
		{
			updatePosition();
		}
	}
	void Text::updatePosition()
	{
		worldVertexArray = vertexArray;
		for (unsigned i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].position.x += position.x;
			worldVertexArray[i].position.y += position.y;
		}

		needPositionUpdate = false;
	}
	void Text::updateText()
	{
		int x = 0.0f;
		int y = ((lineCount - 1) * (font->height + lineSpacing)) * scale;

		textureIDs.clear();
		vertexArray.clear();

		//Iterate through all the characters in the string
		for (unsigned c = 0; c < string.size(); c++)
		{
			if (string[c] == '\n')
			{
				//new line
				x = 0.0f;
				y -= (font->height + lineSpacing) * scale;
			}
			else if (string[c] == '\t')
			{
				//tab
				x += ((font->characters[' '].advance >> 6) * scale) * 3;
			}
			else
			{
				Character ch = font->characters[string[c]];

				if (string[c] == ' ')
				{
					//Don't draw character if it's space, just move x instead
					x += (ch.advance >> 6) * scale; //Bitshift by 6 to get value in pixels (2^6 = 64)
					continue;
				}

				GLfloat xpos = x + ch.bearing.x * scale;
				GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

				GLfloat w = ch.size.x * scale;
				GLfloat h = ch.size.y * scale;

				vertexArray.push_back(Vertex(spehs::vec2(xpos, ypos + h), color, spehs::UV(0.0f, 0.0f)));
				vertexArray.push_back(Vertex(spehs::vec2(xpos, ypos), color, spehs::UV(0.0f, 1.0f)));
				vertexArray.push_back(Vertex(spehs::vec2(xpos + w, ypos), color, spehs::UV(1.0f, 1.0f)));
				vertexArray.push_back(Vertex(spehs::vec2(xpos + w, ypos + h), color, spehs::UV(1.0f, 0.0f)));

				textureIDs.push_back(ch.textureID);

				//Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
				x += (ch.advance >> 6) * scale; //Bitshift by 6 to get value in pixels (2^6 = 64)
			}
		}

		updatePosition();

		needTextUpdate = false;
	}

	void Text::setRenderState(const bool _state)
	{
		renderState = _state;
	}

	void Text::setFont(const std::string &_fontPath, const int &_size)
	{
		if (font)
		{
			FontManager::instance->unreferenceFont(font);
		}

		font = FontManager::instance->getFont(_fontPath, _size);
	}
	void Text::setFont(Font* _font)
	{
		font = _font;
		font->referenceCount++;
	}
	void Text::setFontSize(const int _size)
	{
		//No font loaded
		if (font == nullptr)
			return;

		//The size already matches
		if (font->fontSize == _size)
			return;

		//Get new font face
		FontManager::instance->unreferenceFont(font);
		font = FontManager::instance->getFont(font->fontPath, _size);
	}
	void Text::setString(std::string _str)
	{
		//Set line count
		if (_str.size() > 0) lineCount = 1; else lineCount = 0;
		for (unsigned i = 0; i < _str.size(); i++)
			if (_str[i] == '\n') lineCount++;

		string = _str;
		needTextUpdate = true;
	}
	void Text::setString(const char* _str, const unsigned length)
	{
		//Set line count
		if (length > 0) lineCount = 1; else lineCount = 0;
		for (unsigned i = 0; i < length; i++)
			if (_str[i] == '\n') lineCount++;

		string.resize(length);
		if (length > 0)
			memcpy(&string[0], _str, sizeof(char) * length);
		needTextUpdate = true;
	}
	void Text::incrementString(const std::string _str)
	{
		//Increase line count
		for (unsigned i = 0; i < _str.size(); i++)
			if (_str[i] == '\n') lineCount++;

		string += _str;
		needTextUpdate = true;
	}
	void Text::incrementFrontString(const std::string _str)
	{
		//Increase line count
		for (unsigned i = 0; i < _str.size(); i++)
			if (_str[i] == '\n') lineCount++;

		string = _str + string;
		needTextUpdate = true;
	}
	void Text::setPosition(const spehs::vec2& _vec)
	{
		position.x = _vec.x;
		position.y = _vec.y;
		needPositionUpdate = true;
	}
	void Text::setPosition(const float _x, const float _y)
	{
		position.x = _x;
		position.y = _y;
		needPositionUpdate = true;
	}
	void Text::setPlaneDepth(const PlaneDepth _depth)
	{
		planeDepth = _depth;
	}
	void Text::translate(const spehs::vec2& _vec)
	{
		position.x += _vec.x;
		position.y += _vec.y;
		needPositionUpdate = true;
	}
	void Text::setColor(const spehs::Color& _col)
	{
		color = _col;
		needTextUpdate = true;
	}
	void Text::setAlpha(const spehs::Color::Component& _a)
	{
		color.a = _a;
		needTextUpdate = true;
	}
	void Text::setLineSpacing(const int _lineSpacing)
	{
		lineSpacing = _lineSpacing;
		needTextUpdate = true;
		needPositionUpdate = true;
	}
	int Text::getFontSize() const
	{
		return font->fontSize;
	}
	int Text::getFontHeight() const
	{
		return font->height;
	}
	int Text::getFontAscender() const
	{
		return font->ascender;
	}
	int Text::getFontDescender() const
	{
		return font->descender;
	}
	int Text::getFontMaxAdvanceWidth() const
	{
		return font->maxAdvanceWidth;
	}
	float Text::getX(const int characterIndex) const
	{
		int currentLineWidth(0.0f);
		for (int i = 0; i <= string.size(); i++)
		{
			if (i >= characterIndex)
				break;

			if (string[i] == '\n')
				currentLineWidth = 0.0f;
			else
			{//Increase current line width
#ifdef _DEBUG
				if (font->characters[string[i]].advance > 10000)
					spehs::exceptions::warning("Character width might be invalid!");
				else
#endif
					currentLineWidth += font->characters[string[i]].advance;
			}
		}
		return position.x + (float)(currentLineWidth >> 6);
	}
	float Text::getY(const int characterIndex) const
	{
		const int end = std::min((int)characterIndex, (int)string.size() - 1);
		int currentHeight(0);
		for (int i = string.size() - 1; i >= end; i--)
		{
			if (string[i] == '\n')
				currentHeight += font->height + lineSpacing;
		}
		return position.y + currentHeight;
	}
	float Text::getTextWidth() const
	{
		int record = 0;
		int currentLineWidth = 0;

		for (unsigned i = 0; i < string.size(); i++)
		{
			if (string[i] == '\n')
			{//New line incoming, check current line width and compare with record. Reset current line width.
				if (currentLineWidth > record)
					record = currentLineWidth;
				currentLineWidth = 0;
			}
			else
			{//Increase current line width
#ifdef _DEBUG
				if (font->characters[string[i]].advance > 10000)
				{
					spehs::exceptions::warning("Character width might be invalid!");
					currentLineWidth -= font->characters[string[i]].advance;//Reverse width
				}
#endif
				currentLineWidth += font->characters[string[i]].advance;
			}
		}

		if (currentLineWidth > record)
			return float(currentLineWidth >> 6) * scale;
		return float(record >> 6) * scale;
	}
	float Text::getTextHeight() const
	{
		return ((getFontHeight() + lineSpacing) * (lineCount - 1) + font->ascender - font->descender) * scale;
	}
}