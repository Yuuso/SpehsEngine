#include <algorithm>
#include "SpehsEngine/Core/ApplicationData.h"
#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Rendering/FontManager.h"
#include "SpehsEngine/Rendering/ShaderManager.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/GLSLProgram.h"
#include "SpehsEngine/Rendering/OpenGLError.h"
#include "SpehsEngine/Rendering/FontManager.h"
#include "SpehsEngine/Rendering/Font.h"
#include "SpehsEngine/Rendering/Text.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <freetype/ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <atomic>

std::atomic<int> textAllocations;
std::atomic<int> textDeallocations;


namespace spehs
{
	Text::~Text()
	{
		batchManager.fontManager.unreferenceFont(font);
#ifdef _DEBUG
		textDeallocations++;
#endif
	}

	Text::Text(BatchManager& _batchManager, const PlaneDepth depth)
		: batchManager(_batchManager)
		, lineCount(0)
		, scale(1.0f)
		, lineSpacing(0)
		, font(nullptr)
		, needTextUpdate(false)
		, needPositionUpdate(false)
		, renderState(true)
		, readyForDelete(false)
		, shaderIndex(DefaultText)
		, planeDepth(depth)
		, cameraMatrixState(false)
	{
#ifdef _DEBUG
		textAllocations++;
#endif
	}

	Text::Text(BatchManager& _batchManager, const std::string &_string, const PlaneDepth _depth)
		: Text(_batchManager, _depth)
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
			batchManager.fontManager.unreferenceFont(font);
		}

		font = batchManager.fontManager.getFont(_fontPath, _size);
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
		batchManager.fontManager.unreferenceFont(font);
		font = batchManager.fontManager.getFont(font->fontPath, _size);
	}

	void Text::setString(std::string _str)
	{
		if (_str.size() > 2048)
			spehs::exceptions::unexpectedError("set string is suspiciously big?");
		//Set line count
		if (_str.size() > 0) lineCount = 1; else lineCount = 0;
		for (unsigned i = 0; i < _str.size(); i++)
			if (_str[i] == '\n') lineCount++;

		string = _str;
		needTextUpdate = true;
	}

	void Text::setString(const char* _str, const unsigned length)
	{
		if (length > 2048)
			spehs::exceptions::unexpectedError("set string is suspiciously big?");
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