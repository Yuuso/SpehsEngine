
#pragma once

#include "SpehsEngine/Rendering/Depth.h"
#include "SpehsEngine/Core/Vertex.h"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Core/Vector.h"

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
		spehs::ivec2 size;       // Size of glyph
		spehs::ivec2 bearing;    // Offset from baseline to left/top of glyph
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


	class Text : public Colorable
	{
		friend class BatchManager;
	public:

		void operator=(const Text& _original);

		void destroy();

		void update(); //Called from batch manager when drawing
		void setRenderState(const bool _state);
		bool getRenderState() const { return renderState; }

		void translate(const spehs::vec2& _vec);
		void setPosition(const spehs::vec2& _vec);
		void setPosition(const float x, const float y);

		void setPlaneDepth(const PlaneDepth _depth);

		void setString(const std::string _str);
		void setString(const char* str, const unsigned _length);
		void incrementString(const std::string _str);
		void incrementFrontString(const std::string _str);

		void setFont(const std::string &_fontPath, const int &_size);
		void setFont(Font* _font);
		void setFontSize(const int _size);

		void setColor(const spehs::Color& _newColor);
		void setAlpha(const spehs::Color::Component& _newAlpha);

		void setShader(const int &_shaderIndex);
		void setCameraMatrixState(const bool _state) { cameraMatrixState = _state; }
		void setLineSpacing(const int _lineSpacing);
		void setScale(const float _scale) { scale = _scale; needTextUpdate = true; }
		//Getters
		std::string getString() const { return string; }

		spehs::vec2 getPosition() const { return position; }
		float getX() const { return position.x; }
		float getX(const int characterIndex) const;//Returns x position at given character index. Character width is not included. If index is out of bounds, returns x position of the last character instead.
		float getY() const { return position.y; }
		float getY(const int characterIndex) const;//Returns y position at given character index. Character height is not included. If index is out of bounds, returns y position of the last character instead.

		float getTextWidth() const;
		float getTextHeight() const;
		int getLineCount() const { return lineCount; }
		int getLineSpacing() const { return lineSpacing; }
		float getScale() const { return scale; }

		Font* getFontPtr() const { return font; }
		int getFontSize() const;
		int getFontHeight() const;
		int getFontAscender() const;
		int getFontDescender() const;
		int getFontMaxAdvanceWidth() const;

		int getShaderIndex() const { return shaderIndex; }
		PlaneDepth getPlaneDepth() const { return planeDepth; }
		bool getCameraMatrixState() const { return cameraMatrixState; }

		spehs::Color getColor() const { return color; }
		spehs::Color::Component getAlpha() const { return color.a; }
		
		std::vector<GLuint> textureIDs;
		std::vector<spehs::Vertex> worldVertexArray;
		BatchManager& batchManager;

	protected:
		Text(BatchManager& batchManager, const PlaneDepth _depth);
		Text(BatchManager& batchManager, const std::string &_string, const PlaneDepth _depth);
		//Text(const Text& _original);
		~Text();

	private:
		bool needPositionUpdate;
		bool needTextUpdate;
		
		void updatePosition();
		void updateText();

		std::string string;
		spehs::vec2 position;
		spehs::Color color;
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