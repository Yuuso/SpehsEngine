#include "stdafx.h"
#include "SpehsEngine/Graphics/Text.h"

#include "SpehsEngine/Graphics/Font.h"
#include "SpehsEngine/Graphics/Material.h"
#include <codecvt>


namespace se
{
	namespace graphics
	{
		Text::Text()
			: Primitive()
		{
		}
		Text::~Text()
		{
		}

		void Text::update()
		{
			if (needBufferUpdate)
			{
				generateBuffers();
				needBufferUpdate = false;
			}
			Primitive::update();
		}

		void Text::setLineSpacing(const float _amount)
		{
			lineSpacing = _amount;
			needBufferUpdate = true;
		}
		void Text::setStyle(const TextStyle& _style)
		{
			penPosition.style = _style;
		}
		void Text::movePen(const int _movement)
		{
			if (styledString.empty())
				return;

			penPosition.positionInSegment += _movement;

			// Check if the pen is past the current segment
			while (penPosition.positionInSegment > styledString[penPosition.segmentIndex]->text.size())
			{
				if (styledString.size() == penPosition.segmentIndex)
				{
					// This is the last segment
					penPosition.positionInSegment = styledString[penPosition.segmentIndex]->text.size();
				}
				else
				{
					// Go to next segment
					penPosition.positionInSegment -= styledString[penPosition.segmentIndex]->text.size();
					penPosition.segmentIndex++;
				}
			}

			// Check if the pen is behind the current segment
			while (penPosition.positionInSegment < 0)
			{
				if (penPosition.segmentIndex == 0)
				{
					// This is the first segment
					penPosition.positionInSegment = 0;
				}
				else
				{
					// Go to previous segment
					penPosition.segmentIndex--;
					penPosition.positionInSegment += styledString[penPosition.segmentIndex]->text.size();
				}
			}
		}
		void Text::insert(const std::string& _text)
		{
			std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
			insert(converter.from_bytes(_text));
		}
		void Text::insert(const std::u32string_view _text)
		{
			if (styledString.empty())
			{
				// Make first segment
				styledString.push_back(std::make_unique<StyledStringSegment>());
				styledString.back()->style = penPosition.style;
				styledString.back()->text = _text;

				penPosition.positionInSegment = _text.length();
			}
			else if (styledString[penPosition.segmentIndex]->style == penPosition.style)
			{
				// Insert to current segment
				styledString[penPosition.segmentIndex]->text.insert(penPosition.positionInSegment, _text);

				penPosition.positionInSegment += _text.length();
			}
			else
			{
				if (penPosition.positionInSegment != styledString[penPosition.segmentIndex]->text.length())
				{
					// Split current segment
					auto split = styledString.insert(styledString.begin() + penPosition.segmentIndex + 1, std::make_unique<StyledStringSegment>());
					split->get()->style = styledString[penPosition.segmentIndex]->style;
					split->get()->text = styledString[penPosition.segmentIndex]->text.substr(penPosition.positionInSegment);
				}

				// And a new segment
				penPosition.segmentIndex++;
				auto it = styledString.insert(styledString.begin() + penPosition.segmentIndex, std::make_unique<StyledStringSegment>());
				it->get()->style = penPosition.style;
				it->get()->text = _text;

				penPosition.positionInSegment = _text.length();
			}

			needBufferUpdate = true;
		}
		void Text::clear()
		{
			styledString.clear();
			penPosition.segmentIndex = 0;
			penPosition.positionInSegment = 0;

			needBufferUpdate = true;
		}
		void Text::setPrimitiveType(const PrimitiveType _primitiveType)
		{
			se_assert_m(false, "Cannot change primitive type of Text primitive!");
		}
		void Text::setVertices(const VertexBuffer& _vertices)
		{
			se_assert_m(false, "Cannot set vertices for Text primitive!");
		}
		void Text::setIndices(const std::vector<IndexType>& _indices)
		{
			se_assert_m(false, "Cannot set indices for Text primitive!");
		}

		std::string Text::getPlainText() const
		{
			std::string result;
			std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
			for (auto&& segment : styledString)
			{
				result += converter.to_bytes(segment->text);
			}
			return result;
		}
		const size_t Text::length() const
		{
			size_t result = 0;
			for (auto&& segment : styledString)
			{
				result += segment->text.length();
			}
			return result;
		}
		const TextStyle& Text::getStyle() const
		{
			return penPosition.style;
		}
		const size_t Text::getPenPosition() const
		{
			size_t result = 0;
			for (size_t i = 0; i < penPosition.segmentIndex; i++)
			{
				result += styledString[i]->text.length();
			}
			result += penPosition.positionInSegment;
			return result;
		}
		const float Text::getLineSpacing() const
		{
			return lineSpacing;
		}
		const glm::vec2& Text::getDimensions() const
		{
			// TODO
			static const glm::vec2 temp;
			return temp;
		}

		void Text::generateBuffers()
		{
			std::shared_ptr<Material> primitiveMaterial = getMaterial();
			se_assert(primitiveMaterial);
			if (!primitiveMaterial)
				return;

			std::shared_ptr<Font> font = primitiveMaterial->getFont(0);
			se_assert(font && font->ready());
			if (!font || !font->ready())
				return;

			VertexBuffer newVertices;
			using namespace VertexAttribute;
			newVertices.setAttributes(Position
									  | Color0
									  | TexCoord0);

			glm::vec2 cursor;
			size_t vertexIndex = 0;

			for (auto&& segment : styledString)
			{
				TextStyle& style = segment->style;
				const float atlasSize = (float)font->resourceData->fontMetrics.textureSize;

				for (size_t i = 0; i < segment->text.size(); i++)
				{
					CharacterCode charCode = segment->text[i];
					float advanceMultiplier = 1.0f;

					if (charCode == U'\r')
					{
						// \r\n should be just a single newline, otherwise assume \r == \n
						if ((i + 1) < segment->text.size() && segment->text[i + 1] == '\n')
						{
							continue;
						}
						else
						{
							charCode = U'\n';
						}
					}
					if (charCode == U'\n')
					{
						cursor.x = 0.0f;
						cursor.y += (font->resourceData->fontMetrics.ascent - font->resourceData->fontMetrics.descent) * lineSpacing;
						continue;
					}
					if (charCode == U'\t')
					{
						// Tab counts as 4 spaces
						charCode = U' ';
						advanceMultiplier = 4.0f;
					}

					auto it = font->resourceData->glyphMap.find(charCode);
					if (it == font->resourceData->glyphMap.end())
						charCode = replacementCharacter;

					GlyphMetrics& glyph = font->resourceData->glyphMap[charCode];

					// Glyph
					static constexpr Rectangle emptyRect;
					if (glyph.rectangle != emptyRect)
					{
						newVertices.grow(4);

						for (size_t j = 0; j < 4; j++)
						{
							newVertices.get<Color0>(vertexIndex + j) = style.color;
						}

						newVertices.get<Position>(vertexIndex)	= glm::vec3(cursor.x + glyph.bearingX,							0.0f,			cursor.y + glyph.bearingY);
						newVertices.get<TexCoord0>(vertexIndex) = glm::vec2((float)glyph.rectangle.x / atlasSize,								(float)glyph.rectangle.y / atlasSize);
						vertexIndex++;

						newVertices.get<Position>(vertexIndex)	= glm::vec3(cursor.x + glyph.bearingX + glyph.rectangle.width,	0.0f,			cursor.y + glyph.bearingY);
						newVertices.get<TexCoord0>(vertexIndex) = glm::vec2((float)(glyph.rectangle.x + glyph.rectangle.width) / atlasSize,		(float)glyph.rectangle.y / atlasSize);
						vertexIndex++;

						newVertices.get<Position>(vertexIndex)	= glm::vec3(cursor.x + glyph.bearingX + glyph.rectangle.width,	0.0f,			cursor.y + glyph.bearingY + glyph.rectangle.height);
						newVertices.get<TexCoord0>(vertexIndex) = glm::vec2((float)(glyph.rectangle.x + glyph.rectangle.width) / atlasSize,		(float)(glyph.rectangle.y + glyph.rectangle.height) / atlasSize);
						vertexIndex++;

						newVertices.get<Position>(vertexIndex)	= glm::vec3(cursor.x + glyph.bearingX,							0.0f,			cursor.y + glyph.bearingY + glyph.rectangle.height);
						newVertices.get<TexCoord0>(vertexIndex) = glm::vec2((float)glyph.rectangle.x / atlasSize,								(float)(glyph.rectangle.y + glyph.rectangle.height) / atlasSize);
						vertexIndex++;
					}

					cursor.x += glyph.advanceX * advanceMultiplier;
				}
			}
			Primitive::setVertices(newVertices);

			std::vector<IndexType> newIndices;
			const size_t numVertices = newVertices.size();
			se_assert(numVertices % 4 == 0);
			const size_t numQuads = numVertices / 4;
			newIndices.resize((numVertices - 2) * 3);
			size_t currentIndex = 0;
			for (size_t i = 0; i < numQuads; i++)
			{
				const IndexType currentVertex = (IndexType)i * 4;
				IndexType index = 1;
				newIndices[currentIndex++] = currentVertex;
				newIndices[currentIndex++] = currentVertex + index;
				newIndices[currentIndex++] = currentVertex + (++index);
				newIndices[currentIndex++] = currentVertex;
				newIndices[currentIndex++] = currentVertex + index;
				newIndices[currentIndex++] = currentVertex + (++index);
			}
			Primitive::setIndices(newIndices);
		}
	}
}
