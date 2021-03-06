#pragma once
#include <map>
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include "SpehsEngine/Rendering/Depth.h"
#include "SpehsEngine/Core/Vertex.h"
#include "SpehsEngine/Core/Color.h"

namespace se
{
	namespace rendering
	{
		class Font;
		class BatchManager;
		typedef unsigned int GLuint;
		typedef char GLchar;

		class Text
		{
			friend class BatchManager;
		public:

			void operator=(const Text& _original);

			void destroy();

			void update(); // Called from batch manager when drawing
			void setRenderState(const bool _state);
			bool getRenderState() const { return renderState; }

			void translate(const glm::vec2& _vec);
			void setPosition(const glm::vec2& _vec);
			void setPosition(const float x, const float y);

			void setPlaneDepth(const PlaneDepth _depth);

			void setString(const std::string& _str);
			void setString(const char* str, const unsigned _length);
			void incrementString(const std::string& _str);
			void incrementFrontString(const std::string& _str);

			bool setFont(const std::string& _fontPath, const int _size);
			void setFont(Font* const _font);
			void setFontSize(const int _size);

			void setColor(const Color& _newColor);
			void setAlpha(const float _alpha);

			void setShader(const unsigned int _shaderIndex) { shaderIndex = _shaderIndex; };
			void setCameraMatrixState(const bool _state) { cameraMatrixState = _state; }
			void setLineSpacing(const int _lineSpacing);
			void setScale(const float _scale) { scale = _scale; needTextUpdate = true; }
			//Getters
			std::string getString() const { return string; }

			glm::vec2 getPosition() const { return position; }
			float getX() const { return position.x; }
			float getX(const size_t characterIndex) const;//Returns x position at given character index. Character width is not included. If index is out of bounds, returns x position of the last character instead.
			float getY() const { return position.y; }
			float getY(const size_t characterIndex) const;//Returns y position at given character index. Character height is not included. If index is out of bounds, returns y position of the last character instead.

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

			unsigned int getShaderIndex() const { return shaderIndex; }
			PlaneDepth getPlaneDepth() const { return planeDepth; }
			bool getCameraMatrixState() const { return cameraMatrixState; }

			Color getColor() const { return color; }
			float getAlpha() const { return color.a; }

			std::vector<GLuint> textureIDs;
			std::vector<Vertex> worldVertexArray;
			BatchManager& batchManager;

		protected:
			Text(BatchManager& batchManager, const PlaneDepth _depth);
			Text(BatchManager& batchManager, const std::string &_string, const PlaneDepth _depth);
			//Text(const Text& _original);
			~Text();

		private:

			void updatePosition();
			void updateText();

			std::string string;
			glm::vec2 position;
			Color color;
			float scale = 1.0f;
			int lineCount = 0;
			int lineSpacing = 0;
			PlaneDepth planeDepth = 0;
			unsigned int shaderIndex;
			bool cameraMatrixState = false;
			bool renderState = true;
			bool readyForDelete = false;
			bool needPositionUpdate = false;
			bool needTextUpdate = false;
			std::vector<Vertex> vertexArray;
			Font* font = nullptr;
		};
	}
}