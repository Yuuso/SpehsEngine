#include "stdafx.h"
#include "SpehsEngine/Graphics/Text.h"

#include "SpehsEngine/Core/ColorUtilityFunctions.h"
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
			enableRenderFlags(RenderFlag::Blending);
			disableRenderFlags(RenderFlag::DepthTestLess);
			setName("text");
		}

		void Text::update()
		{
			if (needBufferUpdate)
				generateBuffers();
			Primitive::update();
		}

		void Text::setLineSpacing(const float _amount)
		{
			lineSpacing = _amount;
			textChanged();
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
		void Text::setOrientation(ShapeOrientation _orientation)
		{
			orientation = _orientation;
			needBufferUpdate = true;
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
			textChanged();
		}
		void Text::clear()
		{
			styledString.clear();
			penPosition.segmentIndex = 0;
			penPosition.positionInSegment = 0;
			textChanged();
		}
		void Text::setPrimitiveType(const PrimitiveType _primitiveType)
		{
			se_assert_m(false, "Cannot change primitive type of Text primitive!");
		}
		void Text::setVertices(std::shared_ptr<VertexBuffer> _vertices)
		{
			se_assert_m(false, "Cannot set vertices for Text primitive!");
		}
		void Text::setIndices(std::shared_ptr<IndexBuffer> _indices)
		{
			se_assert_m(false, "Cannot set indices for Text primitive!");
		}
		void Text::setMaterial(std::shared_ptr<Material> _material)
		{
			se_assert(_material);
			Primitive::setMaterial(_material);
			textChanged();
			_material->connectToFontChangedSignal(fontChangedConnection, boost::bind(&Text::fontChanged, this));
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
		const TextDimensions& Text::getDimensions()
		{
			if (needDimensionsUpdate)
			{
				updateDimensions();
			}
			return dimensions;
		}
		ShapeOrientation Text::getOrientation() const
		{
			return orientation;
		}

		void Text::fontChanged()
		{
			fontLoadedConnection.disconnect();
			textChanged();
		}
		void Text::textChanged()
		{
			needBufferUpdate = true;
			needDimensionsUpdate = true;

			// Set null buffers
			// - to disable rendering while font might be loading
			// - to trigger primitive buffer change logic (because text buffers are generated after preRender)
			Primitive::setVertices(nullptr);
			Primitive::setIndices(nullptr);
		}

		void Text::setDebugRendering(bool _value)
		{
			drawDebug = _value;
		}

		void Text::generateBuffers()
		{
			std::shared_ptr<Material> primitiveMaterial = getMaterial();
			se_assert(primitiveMaterial);
			if (!primitiveMaterial)
				return;

			std::shared_ptr<Font> font = primitiveMaterial->getFont(0);
			se_assert(font);
			if (!font)
				return;

			needBufferUpdate = false;

			if (!fontLoadedConnection.connected())
				fontLoadedConnection = font->resourceLoadedSignal.connect(boost::bind(&Text::textChanged, this));
			if (!font->ready())
				return;

			std::shared_ptr<VertexBuffer> newVertexBuffer = std::make_shared<VertexBuffer>();
			VertexBuffer& newVertices = *newVertexBuffer.get();
			using namespace VertexAttribute;
			newVertices.setAttributes(Position
									  | Color0
									  | TexCoord0);

			glm::vec2 cursor;
			size_t vertexIndex = 0;

			const float atlasSize = (float)font->resourceData->fontMetrics.textureSize;

			auto addQuad = [&](const glm::vec2& _offset, const glm::vec2& _size, const Rectangle& _coord, const Color& _color)
				{
					auto orient = [&](float _x, float _y) -> glm::vec3
						{
							switch (orientation)
							{
								case se::graphics::ShapeOrientation::XZ_Plane:
									return glm::vec3(-_x, 0.0f, _y); // TODO: negative X for what reason?
								case se::graphics::ShapeOrientation::XY_Plane:
									return glm::vec3(_x, _y, 0.0f);
							}
							se_assert(false);
							return {};
						};
					auto coords = [&](uint16_t _x, uint16_t _y) -> glm::vec2
						{
							return glm::vec2(static_cast<float>(_x) / atlasSize, static_cast<float>(_y) / atlasSize);
						};

					newVertices.grow(4);

					// CCW frontface

					// Top Left
					newVertices.get<Position>(vertexIndex) = orient(cursor.x + _offset.x, cursor.y + _offset.y);
					newVertices.get<TexCoord0>(vertexIndex) = coords(_coord.x, _coord.y);
					newVertices.get<Color0>(vertexIndex) = _color;
					vertexIndex++;

					// Bottom Left
					newVertices.get<Position>(vertexIndex) = orient(cursor.x + _offset.x, cursor.y + _offset.y - _size.y);
					newVertices.get<TexCoord0>(vertexIndex) = coords(_coord.x, _coord.y + _coord.height);
					newVertices.get<Color0>(vertexIndex) = _color;
					vertexIndex++;

					// Bottom Right
					newVertices.get<Position>(vertexIndex) = orient(cursor.x + _offset.x + _size.x, cursor.y + _offset.y - _size.y);
					newVertices.get<TexCoord0>(vertexIndex) = coords(_coord.x + _coord.width, _coord.y + _coord.height);
					newVertices.get<Color0>(vertexIndex) = _color;
					vertexIndex++;

					// Top Right
					newVertices.get<Position>(vertexIndex) = orient(cursor.x + _offset.x + _size.x, cursor.y + _offset.y);
					newVertices.get<TexCoord0>(vertexIndex) = coords(_coord.x + _coord.width, _coord.y);
					newVertices.get<Color0>(vertexIndex) = _color;
					vertexIndex++;
				};

			if (drawDebug)
			{
				Rectangle& fillerRect = font->resourceData->fillerGlyph;

				// Debug origin
				addQuad(glm::vec2(-3.0f, 0.0f), glm::vec2(7.0f, 1.0f), fillerRect, Color(1.0f, 0.0f, 0.0f));
				addQuad(glm::vec2(0.0f, 3.0f), glm::vec2(1.0f, 7.0f), fillerRect, Color(1.0f, 0.0f, 0.0f));

				// Debug dimensions
				const TextDimensions textDimensions = getDimensions();
				addQuad(textDimensions.offsetFromOrigin, textDimensions.dimensions, fillerRect, randomBrightColor().withAlpha(0.2f));
			}

			for (auto&& segment : styledString)
			{
				TextStyle& style = segment->style;

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
						cursor.y -= font->resourceData->fontMetrics.height * lineSpacing;
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
						addQuad(glm::vec2(glyph.bearingX, glyph.bearingY), glm::vec2(glyph.rectangle.width, glyph.rectangle.height), glyph.rectangle, style.color);
					}

					cursor.x += glyph.advanceX * advanceMultiplier;
				}
			}
			Primitive::setVertices(newVertexBuffer);

			std::shared_ptr<IndexBuffer> newIndexBuffer = std::make_shared<IndexBuffer>();
			IndexBuffer& newIndices = *newIndexBuffer.get();
			const size_t numVertices = newVertices.size();
			if (numVertices > 0)
			{
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
			}
			Primitive::setIndices(newIndexBuffer);
		}

		void Text::updateDimensions()
		{
			std::shared_ptr<Material> primitiveMaterial = getMaterial();
			if (!primitiveMaterial)
			{
				log::warning("Text::updateDimensions: Missing material!");
				return;
			}

			std::shared_ptr<Font> font = primitiveMaterial->getFont(0);
			if (!font || !font->ready())
			{
				log::warning("Text::updateDimensions: Missing font!");
				return;
			}

			dimensions = TextDimensions();

			glm::vec2 cursor;

			for (auto&& segment : styledString)
			{
				//TextStyle& style = segment->style;

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
						if (cursor.x == 0.0f && cursor.y == 0.0f)
						{
							// Empty first line
							dimensions.offsetFromOrigin.y = static_cast<float>(font->resourceData->fontMetrics.ascent);
						}
						cursor.x = 0.0f;
						cursor.y -= font->resourceData->fontMetrics.height * lineSpacing;
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

					if (cursor.x == 0.0f && cursor.y == 0.0f)
					{
						dimensions.offsetFromOrigin.x = static_cast<float>(glyph.bearingX);
						dimensions.offsetFromOrigin.y = static_cast<float>(glyph.bearingY);
					}

					cursor.x += glyph.advanceX * advanceMultiplier;

					dimensions.dimensions.x = std::max(dimensions.dimensions.x, cursor.x);
					dimensions.dimensions.y = std::max(dimensions.dimensions.y, -((cursor.y + glyph.bearingY) - glyph.rectangle.height));
				}
			}
			dimensions.dimensions.x -= dimensions.offsetFromOrigin.x;
			dimensions.dimensions.y += dimensions.offsetFromOrigin.y;
			needDimensionsUpdate = false;
		}
	}
}
