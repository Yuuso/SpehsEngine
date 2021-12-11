#include "stdafx.h"
#include "SpehsEngine/GUI/GUIShape.h"

#include "SpehsEngine/Graphics/Scene.h"
#include "SpehsEngine/GUI/GUIView.h"
#include "SpehsEngine/Math/GLMMatrixUtilityFunctions.h"


namespace se
{
	using namespace graphics;

	namespace gui
	{
		GUIShape::GUIShape()
		{
			initShape();

			// Default non-zero size
			setSize(GUIVec2({ 10.0f, 10.0f }, GUIUnitType::Pixels));
		}
		GUIShape::GUIShape(const GUIShape& _other)
			: GUIElement(_other)
			, textureName(_other.textureName)
		{
			initShape();
			setColor(_other.getColor());
			setShapeType(_other.getShapeType());
		}

		std::shared_ptr<GUIElement> GUIShape::clone()
		{
			return std::make_shared<GUIShape>(*this);
		}


		void GUIShape::initShape()
		{
			ShapeParameters params;
			params.orientation = ShapeOrientation::XY_Plane;

			shape.generate(ShapeType::Rectangle, params);
			enableBit(updateFlags, GUIElementUpdateFlag::MaterialUpdateNeeded);
		}

		void GUIShape::elementUpdate(UpdateContext& _context)
		{
			if (checkBit(updateFlags, GUIElementUpdateFlag::MaterialUpdateNeeded))
			{
				if (!textureName.empty())
				{
					shape.setMaterial(_context.materialManager.createTextureMaterial(textureName));
				}
				else
				{
					shape.setMaterial(_context.materialManager.createColorMaterial());
				}
			}
			if (checkBit(updateFlags, GUIElementUpdateFlag::PrimitiveAddNeeded))
			{
				_context.scene.add(shape);
			}
			if (checkBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded))
			{
				shape.setRenderState(globalVisible);
				if (globalVisible)
				{
					glm::vec3 globalPosition;
					glm::quat globalRotation;
					glm::vec3 globalScale;
					decomposeTransformationMatrix(globalTrasform, &globalPosition, &globalRotation, &globalScale);

					shape.setPosition(globalPosition);
					shape.setRotation(globalRotation);
					shape.setScale(globalScale * glm::vec3(unitToPixels(getSize(), _context.viewSize), 1.0f));

					shape.setScissor(globalScissor);

					if (getColor().a < 1.0f || !textureName.empty())
						shape.setRenderFlags(RenderFlag::BlendAlpha | RenderFlag::DepthTestLess);
					else
						shape.setRenderFlags(RenderFlag::WriteDepth | RenderFlag::WriteAlpha | RenderFlag::DepthTestLess);
				}
			}
		}
		void GUIShape::onAddedParent()
		{
			enableBit(updateFlags, GUIElementUpdateFlag::PrimitiveAddNeeded);
			GUIElement::onAddedParent();
		}
		void GUIShape::onRemovedParent()
		{
			shape.removeFromScenes();
			GUIElement::onRemovedParent();
		}
		void GUIShape::onRemovedFromView()
		{
			shape.removeFromScenes();
			GUIElement::onRemovedFromView();
		}
		void GUIShape::onAddedToView()
		{
			enableBit(updateFlags, GUIElementUpdateFlag::PrimitiveAddNeeded);
			GUIElement::onAddedToView();
		}
		GUIVec2 GUIShape::getTransformOffset()
		{
			return GUIVec2(GUIUnit(getSize().x.value * 0.5f, getSize().x.type), GUIUnit(getSize().y.value * 0.5f, getSize().y.type));
		}

		ShapeType GUIShape::getShapeType() const
		{
			return shape.getShapeType();
		}
		const Color& GUIShape::getColor() const
		{
			return shape.getColor();
		}

		void GUIShape::setShapeType(ShapeType _type)
		{
			if (shape.getShapeType() == _type)
				return;
			ShapeParameters params;
			params.orientation = ShapeOrientation::XY_Plane;
			shape.generate(_type, params);
		}
		void GUIShape::setColor(const Color& _color)
		{
			shape.setColor(_color);
		}
		void GUIShape::setTexture(std::string_view _name)
		{
			textureName = _name;
			enableBit(updateFlags, GUIElementUpdateFlag::MaterialUpdateNeeded);
		}
	}
}
