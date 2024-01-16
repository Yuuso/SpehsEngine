#pragma once

#include "SpehsEngine/Graphics/Line.h"
#include "SpehsEngine/Graphics/Model.h"
#include "SpehsEngine/Graphics/Shape.h"
#include "SpehsEngine/Graphics/Text.h"
#include "SpehsEngine/Graphics/Scene.h"
#include "SpehsEngine/Graphics/View.h"


namespace se
{
	class AssetManager;
	namespace gfx
	{
		class ShapeGenerator;
	}

	namespace debug
	{
		template <typename T>
		class GraphicsObjectModifier
		{
		public:
			GraphicsObjectModifier() = delete;
			GraphicsObjectModifier(const GraphicsObjectModifier&) = delete;
			GraphicsObjectModifier& operator=(const GraphicsObjectModifier&) = delete;
			GraphicsObjectModifier& operator=(GraphicsObjectModifier&&) = delete;

			GraphicsObjectModifier(T& _object)
				: object(_object) {}
			~GraphicsObjectModifier() = default;
			GraphicsObjectModifier(GraphicsObjectModifier&&) = default;

			GraphicsObjectModifier<T>& position(const glm::vec3& _position)							{ object.setPosition(_position); return *this; }
			GraphicsObjectModifier<T>& scale(const glm::vec3& _scale)								{ object.setScale(_scale); return *this; }
			GraphicsObjectModifier<T>& rotation(const glm::quat& _rotation)							{ object.setRotation(_rotation); return *this; }
			GraphicsObjectModifier<T>& color(const se::Color& _color)								{ object.setColor(_color); return *this; }
			GraphicsObjectModifier<T>& material(std::shared_ptr<se::gfx::Material> _material)	{ object.setMaterial(_material); return *this; }
			GraphicsObjectModifier<T>& primitiveType(se::gfx::PrimitiveType _primitiveType)	{ object.setPrimitiveType(_primitiveType); return *this; }
			GraphicsObjectModifier<T>& renderFlags(se::gfx::RenderFlagsType _renderFlags)		{ object.setRenderFlags(_renderFlags); return *this; }

		private:
			T& object;
		};

		template <typename T>
		struct FrameVector
		{
			FrameVector(se::gfx::View& _view) : view(_view) {}
			T& find(std::string_view _name)
			{
				auto it = std::find_if(vector.begin(), vector.end(), [_name](std::unique_ptr<T>& _e){ return _e->getName() == _name; });
				if (it != vector.end())
				{
					it->get()->setRenderState(true);
					return **it;
				}
				T& result = next();
				result.setName(_name);
				return result;
			}
			T& next()
			{
				se_assert(vector.size() >= currentIndex);
				if (vector.size() <= currentIndex)
				{
					vector.push_back(std::make_unique<T>());
					view.getScene().add(*vector.back());
				}
				T* result = vector[currentIndex++].get();
				se_assert(result);
				result->setRenderState(true);
				return *result;
			}
			void endFrame()
			{
				for (auto&& element : vector)
				{
					element->setRenderState(false);
				}
				currentIndex = 0;
			}
			void clear()
			{
				vector.clear();
				currentIndex = 0;
			}

			se::gfx::View& view;
			std::vector<std::unique_ptr<T>> vector;
			size_t currentIndex = 0;
		};

		typedef GraphicsObjectModifier<se::gfx::Primitive> PrimitiveModifier;
		typedef GraphicsObjectModifier<se::gfx::Model> ModelModifier;

		class ImmediateModeGraphics
		{
		public:

			ImmediateModeGraphics(
				se::gfx::View& _view,
				se::AssetManager& _assetManager,
				se::gfx::ShapeGenerator& shapeGenerator);

			void init();
			void endFrame();
			void cleanup();

			PrimitiveModifier	shape(se::gfx::ShapeType _type, se::gfx::ShapeParameters _params = {});
			PrimitiveModifier	icon(std::string_view _texture);
			PrimitiveModifier	line(const glm::vec3& _start, const glm::vec3& _end);
			PrimitiveModifier	linePoint(const glm::vec3& _point, std::string_view _lineId);
			PrimitiveModifier	text(const std::string& _text, std::string_view _font = "");
			ModelModifier		model(std::string_view _model);

		private:

			se::gfx::View& view;
			se::AssetManager& assetManager;
			se::gfx::ShapeGenerator& shapeGenerator;

			std::shared_ptr<se::gfx::Material> defaultFlatMaterial;
			std::shared_ptr<se::gfx::Material> defaultTextMaterial;

			FrameVector<se::gfx::Shape> shapes;
			FrameVector<se::gfx::Line> lines;
			FrameVector<se::gfx::Text> texts;
			FrameVector<se::gfx::Model> models;
		};
	}
}
