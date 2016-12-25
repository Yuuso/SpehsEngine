
#pragma once

#include "Depth.h"
#include "Vertex.h"
#include "CommonColor.h"

#include <glm/glm.hpp>

#include <map>
#include <string>
#include <vector>


typedef unsigned int GLuint;
typedef char GLchar;

class BatchManager;


namespace spehs
{
	struct Character
	{
		GLuint     textureID;  // ID handle of the glyph texture
		glm::ivec2 size;       // Size of glyph
		glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
		GLuint     advance;    // Offset to advance to next glyph
	};


	struct Font;

	class FontManager
	{
	public:
		static FontManager* instance;

		FontManager();
		~FontManager();

		Font* getFont(const std::string &_fontPath, const int &_size);
		void unreferenceFont(Font* _font);

	private:
		std::vector<Font*> fonts;
	};


	class Text
	{
		friend class BatchManager;

	public:
		static Text* create(const PlaneDepth &_depth = DEPTH_MAX);
		static Text* create(const std::string &_string, const PlaneDepth &_depth = DEPTH_MAX);

		void operator=(const Text& _original);

		void destroy();

		void update(); //Called from batch manager when drawing
		void setRenderState(const bool _state);
		bool getRenderState() const { return renderState; }

		void translate(const glm::vec2& _vec);
		void setPosition(const glm::vec2& _vec);
		void setPosition(const float x, const float y);

		void setPlaneDepth(const PlaneDepth _depth);

		void setString(const std::string _str);
		void setString(const char* str, const unsigned _length);
		void incrementString(const std::string _str);
		void incrementFrontString(const std::string _str);

		void setFont(const std::string &_fontPath, const int &_size);
		void setFont(Font* _font);
		void setFontSize(const int _size);

		void setColor(const glm::vec4& _col);
		void setColor(const CommonColor& _col);
		void setColor(const float _red, const float _green, const float _blue, const float _alpha = 1.0f);
		void setColor(const unsigned char _r, const unsigned char _g, const unsigned char _b, const unsigned char _a = 255);
		void setAlpha(const float _alpha);
		void setAlpha(const unsigned char _a);

		void setShader(const int &_shaderIndex);
		void setCameraMatrixState(const bool _state) { cameraMatrixState = _state; }
		void setLineSpacing(const int _lineSpacing);

		//Getters
		std::string getString() const { return string; }

		glm::vec2 getPosition() const { return position; }
		float getX() const { return position.x; }
		float getX(unsigned characterIndex) const;//Returns x position at given character index. Character width is not included. If index is out of bounds, returns x position of the last character instead.
		float getY() const { return position.y; }

		int getTextWidth() const;
		int getTextHeight() const;
		int getLineCount() const { return lineCount; }
		int getLineSpacing() const { return lineSpacing; }

		Font* getFontPtr() const { return font; }
		int getFontSize() const;
		int getFontHeight() const;
		int getFontAscender() const;
		int getFontDescender() const;

		int getShaderIndex() const { return shaderIndex; }
		PlaneDepth getPlaneDepth() const { return planeDepth; }
		bool getCameraMatrixState() const { return cameraMatrixState; }

		glm::vec4 getColor() const { return color.rgba; }

		std::vector<GLuint> textureIDs;
		std::vector<spehs::Vertex> worldVertexArray;

	protected:
		Text(const PlaneDepth _depth);
		Text(const std::string &_string, const PlaneDepth _depth);
		//Text(const Text& _original);
		~Text();

	private:
		bool needPositionUpdate;
		bool needTextUpdate;
		
		void updatePosition();
		void updateText();

		std::string string;
		glm::vec2 position;
		spehs::ColorRGBA color;
		float scale;

		int lineCount;
		int lineSpacing;

		PlaneDepth planeDepth;
		int shaderIndex;
		bool cameraMatrixState;
		bool renderState;
		bool readyForDelete;

		std::vector<spehs::Vertex> vertexArray;

		Font* font;
	};
}