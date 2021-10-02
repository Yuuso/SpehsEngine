#pragma once

#include "boost/signals2.hpp"
#include "SpehsEngine/Graphics/CharacterSet.h"
#include "SpehsEngine/Graphics/Internal/FontMetrics.h"
#include "SpehsEngine/Graphics/Primitive.h"
#include "SpehsEngine/Graphics/TextStyle.h"
#include "SpehsEngine/Graphics/Types.h"
#include <memory>
#include <string>
#include <vector>


namespace se
{
	namespace graphics
	{
		class Text : public Primitive
		{
		public:

									Text();
									~Text() = default;

									Text(const Text& _other) = delete;
			Text&					operator=(const Text& _other) = delete;

									Text(Text&& _other) = delete;
			Text&					operator=(Text&& _other) = delete;


			void					setLineSpacing(const float _amount);
			void					setStyle(const TextStyle& _style);
			void					movePen(const int _movement);

			void					insert(const std::string& _text);			// utf-8
			void					insert(const std::u32string_view _text);	// utf-32
			void					clear();

			void					setPrimitiveType(const PrimitiveType _primitiveType) override;
			void					setVertices(std::shared_ptr<VertexBuffer> _vertices) override;
			void					setIndices(std::shared_ptr<IndexBuffer> _indices) override;
			void					setMaterial(std::shared_ptr<Material> _material) override;


			const size_t			length() const;
			std::string				getPlainText() const;
			const TextStyle&		getStyle() const;
			const size_t			getPenPosition() const;
			const float				getLineSpacing() const;
			const TextDimensions&	getDimensions();

		protected:

			virtual void			update() override;

		private:

			void					fontChanged();
			void					textChanged();
			void					generateBuffers();
			void					updateDimensions();

			StyledString styledString;

			struct PenPosition
			{
				TextStyle style;
				size_t segmentIndex = 0;
				size_t positionInSegment = 0;
			};
			PenPosition penPosition;

			TextDimensions dimensions;
			float lineSpacing = 1.0f;
			bool needDimensionsUpdate = false;
			bool needBufferUpdate = false;

			boost::signals2::scoped_connection fontLoadedConnection;
			boost::signals2::scoped_connection fontChangedConnection;
		};
	}
}
