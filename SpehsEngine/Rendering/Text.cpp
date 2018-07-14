#include "stdafx.h"
#include <algorithm>
#include "SpehsEngine/Core/Log.h"
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


namespace se
{
	namespace rendering
	{
		Text::~Text()
		{
#ifdef _DEBUG
			textDeallocations++;
#endif
		}

		Text::Text(BatchManager& _batchManager, const PlaneDepth depth)
			: batchManager(_batchManager)
			, planeDepth(depth)
		{
#ifdef _DEBUG
			textAllocations++;
#endif
			shaderIndex = (unsigned int)ShaderName::DefaultText;
		}

		Text::Text(BatchManager& _batchManager, const std::string &_string, const PlaneDepth _depth)
			: Text(_batchManager, _depth)
		{
			setString(_string);
		}

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
			if (!font)
			{
				log::warning("Text::updateText(): no font set");
				return;
			}
			float x = 0.0f;
			float y = ((lineCount - 1) * (font->height + lineSpacing)) * scale;

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
					x += (float(font->characters[' '].advance >> 6) * scale) * 4.0f;
				}
				else
				{
					Character ch = font->characters[string[c]];

					if (string[c] == ' ')
					{
						//Don't draw character if it's space, just move x instead
						x += float(ch.advance >> 6) * scale; //Bitshift by 6 to get value in pixels (2^6 = 64)
						continue;
					}

					GLfloat xpos = std::round(x + ch.bearing.x * scale);
					GLfloat ypos = std::round(y - (ch.size.y - ch.bearing.y) * scale);
					GLfloat w = std::round(ch.size.x * scale);
					GLfloat h = std::round(ch.size.y * scale);

					vertexArray.push_back(se::Vertex(se::vec2(xpos, ypos + h), color, se::UV(0.0f, 0.0f)));
					vertexArray.push_back(se::Vertex(se::vec2(xpos, ypos), color, se::UV(0.0f, 1.0f)));
					vertexArray.push_back(se::Vertex(se::vec2(xpos + w, ypos), color, se::UV(1.0f, 1.0f)));
					vertexArray.push_back(se::Vertex(se::vec2(xpos + w, ypos + h), color, se::UV(1.0f, 0.0f)));

					textureIDs.push_back(ch.textureID);

					//Advance cursor for the next glyph (note that advance is number of 1/64 pixels)
					x += float(ch.advance >> 6) * scale; //Bitshift by 6 to get value in pixels (2^6 = 64)
				}
			}

			updatePosition();

			needTextUpdate = false;
		}

		void Text::setRenderState(const bool _state)
		{
			renderState = _state;
		}

		void Text::setFont(const std::string& _fontPath, const int _size)
		{
			setFont(batchManager.fontManager.getFont(_fontPath, _size));
		}

		void Text::setFont(Font *fontParam)
		{
			if (font == fontParam)
				return;
			font = fontParam;
			needTextUpdate = true;
			needPositionUpdate = true;
		}

		void Text::setFontSize(const int sizeParam)
		{
			if (!font)
			{
				log::warning("Text::setFontSize(): no font set");
				return;
			}

			//Check if the size already matches
			if (font->fontSize == sizeParam)
				return;

			//Set font
			setFont(font->fontPath, sizeParam);
		}

		void Text::setString(const std::string& _str)
		{
			if (_str == string)
				return;
			if (_str.size() > 2048)
				log::error("set string is suspiciously big?");

			//Update line count
			if (_str.size() == 0)
				lineCount = 0;
			else
				lineCount = 1;
			for (unsigned i = 0; i < _str.size(); i++)
			{
				if (_str[i] == '\n')
					lineCount++;
			}

			string = _str;
			needTextUpdate = true;
		}

		void Text::setString(const char* _str, const unsigned length)
		{
			if (length == string.size() && (length == 0 || memcmp(string.c_str(), _str, length) == 0))
				return;
			if (length > 2048)
				log::error("set string is suspiciously big?");

			//Update line count
			if (length == 0)
				lineCount = 0;
			else
				lineCount = 1;
			for (unsigned i = 0; i < length; i++)
			{
				if (_str[i] == '\n')
					lineCount++;
			}

			string.resize(length);
			if (length > 0)
				memcpy(&string[0], _str, sizeof(char) * length);
			needTextUpdate = true;
		}

		void Text::incrementString(const std::string& _str)
		{
			if (_str.empty())
				return;

			//Increase line count
			for (unsigned i = 0; i < _str.size(); i++)
				if (_str[i] == '\n') lineCount++;

			string += _str;
			needTextUpdate = true;
		}

		void Text::incrementFrontString(const std::string& _str)
		{
			if (_str.empty())
				return;

			//Increase line count
			for (unsigned i = 0; i < _str.size(); i++)
				if (_str[i] == '\n') lineCount++;

			string = _str + string;
			needTextUpdate = true;
		}

		void Text::setPosition(const se::vec2& _vec)
		{
			if (position.x == _vec.x && position.y == _vec.y)
				return;
			position = _vec;
			needPositionUpdate = true;
		}

		void Text::setPosition(const float _x, const float _y)
		{
			if (position.x == _x && position.y == _y)
				return;
			position.x = _x;
			position.y = _y;
			needPositionUpdate = true;
		}

		void Text::setPlaneDepth(const PlaneDepth _depth)
		{
			planeDepth = _depth;
		}

		void Text::translate(const se::vec2& _vec)
		{
			position.x += _vec.x;
			position.y += _vec.y;
			needPositionUpdate = true;
		}

		void Text::setColor(const se::Color& colorParam)
		{
			if (color == colorParam)
				return;
			color = colorParam;
			needTextUpdate = true;
		}

		void Text::setAlpha(const float _alpha)
		{
			if (color.a == _alpha)
				return;
			color.a = _alpha;
			needTextUpdate = true;
		}

		void Text::setLineSpacing(const int lineSpacingParam)
		{
			if (lineSpacing == lineSpacingParam)
				return;
			lineSpacing = lineSpacingParam;
			needTextUpdate = true;
			needPositionUpdate = true;
		}

		int Text::getFontSize() const
		{
			if (font)
				return font->fontSize;
			else
			{
				log::warning("Text::getFontSize(): no font set");
				return 0;
			}
		}

		int Text::getFontHeight() const
		{
			if (font)
				return font->height;
			else
			{
				log::warning("Text::getFontHeight(): no font set");
				return 0;
			}
		}

		int Text::getFontAscender() const
		{
			if (font)
				return font->ascender;
			else
			{
				log::warning("Text::getFontAscender(): no font set");
				return 0;
			}
		}

		int Text::getFontDescender() const
		{
			if (font)
				return font->descender;
			else
			{
				log::warning("Text::getFontDescender(): no font set");
				return 0;
			}
		}

		int Text::getFontMaxAdvanceWidth() const
		{
			if (font)
				return font->maxAdvanceWidth;
			else
			{
				log::warning("Text::getFontMaxAdvanceWidth(): no font set");
				return 0;
			}
		}

		float Text::getX(const size_t characterIndex) const
		{
			if (!font)
			{
				log::warning("Text::getX(): no font set");
				return 0.0f;
			}
			int currentLineWidth = 0;
			for (size_t i = 0; i <= string.size(); i++)
			{
				if (i >= characterIndex)
					break;

				if (string[i] == '\n')
					currentLineWidth = 0;
				else
				{//Increase current line width
#ifdef _DEBUG
					if (font->characters[string[i]].advance > 10000)
						log::warning("Character width might be invalid!");
					else
#endif
						currentLineWidth += font->characters[string[i]].advance;
				}
			}
			return position.x + (float)(currentLineWidth >> 6);
		}

		float Text::getY(const size_t characterIndex) const
		{
			if (!font)
			{
				log::warning("Text::getY(): no font set");
				return 0.0f;
			}
			const int end = std::min((int)characterIndex, (int)string.size() - 1);
			int currentHeight = 0;
			for (int i = int(string.size()) - 1; i >= end; i--)
			{
				if (string[i] == '\n')
					currentHeight += font->height + lineSpacing;
			}
			return position.y + float(currentHeight);
		}

		float Text::getTextWidth() const
		{
			if (!font)
			{
				log::warning("Text::getTextWidth(): no font set");
				return 0.0f;
			}
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
						log::warning("Character width might be invalid!");
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
			if (!font)
			{
				log::warning("Text::getX(): no font set");
				return 0.0f;
			}
			return float((getFontHeight() + lineSpacing) * (lineCount - 1) + font->ascender - font->descender) * scale;
		}
	}
}