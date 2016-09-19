
#pragma once

#include "Depth.h"
#include "Vertex.h"

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
		void operator=(const Text& _original);

		void destroy();

		void update(); //Called from batch manager when drawing
		void setRenderState(bool _state);

		void translate(glm::vec2& _vec);
		void setPosition(glm::vec2& _vec);
		void setPosition(float, float);

		void setPlaneDepth(PlaneDepth _depth);

		void setString(std::string _str);
		void setString(char* str, unsigned _length);
		void incrementString(std::string _str);
		void incrementFrontString(std::string _str);

		void setFont(const std::string &_fontPath, const int &_size);
		void setFont(Font* _font);
		void setFontSize(int _size);

		void setColor(glm::vec4& _col);
		void setColor(float _red, float _green, float _blue, float _alpha = 1.0f);
		void setColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 1.0f);
		void setAlpha(float _alpha);
		void setAlpha(unsigned char _a);

		void setShader(const int &_shaderIndex);

		//Getters
		std::string getString(){ return string; }

		int getTextWidth() const;
		int getTextHeight() const;
		int getLineCount() const { return lineCount; }

		Font* getFontPtr() const { return font; }
		int getFontSize() const;
		int getFontHeight() const;
		int getFontDescender() const;

		int getShaderIndex() const { return shaderIndex; }
		PlaneDepth getPlaneDepth() const { return planeDepth; }
		bool getCameraMatrixState() const { return cameraMatrixState; }

		glm::vec4 getColor() const { return color; }

		std::vector<GLuint> textureIDs;
		std::vector<spehs::Vertex> worldVertexArray;

	protected:
		Text(PlaneDepth _depth);
		//Text(const Text& _original);
		~Text();

	private:
		bool needPositionUpdate;
		bool needTextUpdate;
		
		void updatePosition();
		void updateText();

		std::string string;
		glm::vec2 position;
		glm::vec4 color;
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