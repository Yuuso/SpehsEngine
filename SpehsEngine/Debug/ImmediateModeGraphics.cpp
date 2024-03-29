#include "stdafx.h"
#include "SpehsEngine/Debug/ImmediateModeGraphics.h"

#include "SpehsEngine/Graphics/DefaultMaterials.h"
#include "SpehsEngine/Graphics/FontManager.h"
#include "SpehsEngine/Graphics/Line.h"
#include "SpehsEngine/Graphics/Material.h"
#include "SpehsEngine/Graphics/Model.h"
#include "SpehsEngine/Graphics/ModelDataManager.h"
#include "SpehsEngine/Graphics/ShaderManager.h"
#include "SpehsEngine/Graphics/Shape.h"
#include "SpehsEngine/Graphics/ShapeGenerator.h"
#include "SpehsEngine/Graphics/Text.h"
#include "SpehsEngine/Graphics/TextureManager.h"

using namespace se::graphics;


namespace se
{
	namespace debug
	{
		template <typename T>
		static void setDefaults(T& _type)
		{
			_type.setPosition(glm::vec3{});
			_type.setScale(glm::vec3(1.0f));
			_type.setRotation(glm::identity<glm::quat>());
			_type.setColor(se::Color{});
			_type.setRenderFlags(defaultRenderFlags);
		}

		ImmediateModeGraphics::ImmediateModeGraphics(
				View& _view, ShaderManager& _shaderManager,
				TextureManager& _textureManager, FontManager& _fontManager,
				ModelDataManager& _modelDataManager, ShapeGenerator& _shapeGenerator)
			: view(_view)
			, shaderManager(_shaderManager)
			, textureManager(_textureManager)
			, fontManager(_fontManager)
			, modelDataManager(_modelDataManager)
			, shapeGenerator(_shapeGenerator)
			, shapes(_view)
			, lines(_view)
			, texts(_view)
			, models(_view)
		{}

		void ImmediateModeGraphics::init()
		{
			defaultFlatMaterial = createMaterial(DefaultMaterialType::FlatColor, shaderManager);
			defaultTextMaterial = createMaterial(DefaultMaterialType::Text, shaderManager);
			defaultTextMaterial->setFont(fontManager.getDefaultFont());
		}
		void ImmediateModeGraphics::endFrame()
		{
			shapes.endFrame();
			lines.endFrame();
			texts.endFrame();
			models.endFrame();

			for (auto && line : lines.vector)
			{
				line->clear();
				setDefaults(*line);
				line->setMaterial(defaultFlatMaterial);
			}
		}
		void ImmediateModeGraphics::cleanup()
		{
			shapes.clear();
			lines.clear();
			texts.clear();
			models.clear();
		}

		PrimitiveModifier ImmediateModeGraphics::shape(ShapeType _type, ShapeParameters _params)
		{
			Shape& shape = shapes.next();
			shape.generate(_type, _params, &shapeGenerator);
			setDefaults(shape);
			shape.setPrimitiveType(PrimitiveType::Triangles);
			shape.setMaterial(defaultFlatMaterial);
			return shape;
		}
		PrimitiveModifier ImmediateModeGraphics::icon(std::string_view _texture)
		{
			Shape& shape = shapes.next();
			shape.generate(ShapeType::Square, ShapeParameters{}, &shapeGenerator);
			setDefaults(shape);
			auto material = createMaterial(DefaultMaterialType::FlatTexture, shaderManager);
			auto texture = textureManager.find(_texture);
			if (!texture)
			{
				texture = textureManager.create(_texture, _texture);
			}
			material->setTexture(texture);
			shape.setMaterial(material);
			shape.setPrimitiveType(PrimitiveType::Triangles);
			shape.enableRenderFlags(RenderFlag::BillboardSpherical);
			return shape;
		}
		PrimitiveModifier ImmediateModeGraphics::line(const glm::vec3& _start, const glm::vec3& _end)
		{
			Line& line = lines.next();
			line.clear();
			line.addPoint(_start);
			line.addPoint(_end);
			setDefaults(line);
			line.setMaterial(defaultFlatMaterial);
			return line;
		}
		PrimitiveModifier ImmediateModeGraphics::linePoint(const glm::vec3& _point, std::string_view _lineId)
		{
			se_assert(!_lineId.empty());
			Line& line = lines.find(_lineId);
			line.addPoint(_point);
			return line;
		}
		PrimitiveModifier ImmediateModeGraphics::text(const std::string& _text, std::string_view _font)
		{
			Text& text = texts.next();
			text.clear();
			text.insert(_text);
			setDefaults(text);
			if (_font.empty())
			{
				text.setMaterial(defaultTextMaterial);
			}
			else
			{
				auto material = createMaterial(DefaultMaterialType::Text, shaderManager);
				auto font = fontManager.find(_font);
				if (!font)
				{
					font = fontManager.create(_font, _font, 30);
				}
				material->setFont(font);
				text.setMaterial(material);
			}
			return text;
		}
		ModelModifier ImmediateModeGraphics::model(std::string_view _model)
		{
			Model& model = models.next();
			auto modelData = modelDataManager.find(_model);
			if (!modelData)
			{
				modelData = modelDataManager.create(_model, _model);
			}
			model.loadModelData(modelData);
			setDefaults(model);
			model.setPrimitiveType(PrimitiveType::Triangles);
			model.setMaterial(defaultFlatMaterial);
			return model;
		}
	}
}
