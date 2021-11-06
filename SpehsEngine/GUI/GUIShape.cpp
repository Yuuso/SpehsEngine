#include "stdafx.h"
#include "SpehsEngine/GUI/GUIShape.h"

#include "SpehsEngine/GUI/GUIView.h"
#include "SpehsEngine/Math/GLMMatrixUtilityFunctions.h"


namespace se
{
	using namespace graphics;

	namespace gui
	{
		GUIShape::GUIShape()
		{
			ShapeParameters params;
			params.orientation = ShapeOrientation::XY_Plane;

			shape.generate(ShapeType::Rectangle, params);
			shape.setRenderFlags(RenderFlag::Blending | RenderFlag::DepthTestLess | RenderFlag::DepthWrite);
			enableBit(updateFlags, GUIElementUpdateFlag::MaterialUpdateNeeded);

			// Default non-zero size
			setSize(GUIVec2({ 10.0f, 10.0f }, GUIUnitType::Pixels));
		}

		void GUIShape::elementUpdate(UpdateContext& _context)
		{
			if (checkBit(updateFlags, GUIElementUpdateFlag::MaterialUpdateNeeded))
			{
				// TODO
				se_assert(false);
			}
			if (checkBit(updateFlags, GUIElementUpdateFlag::PrimitiveAddNeeded))
			{
				_context.view.getView().getScene().add(shape);
			}
			if (checkBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded))
			{
				glm::vec3 globalPosition;
				glm::quat globalRotation;
				glm::vec3 globalScale;
				decomposeTransformationMatrix(globalTrasform, &globalPosition, &globalRotation, &globalScale);

				shape.setPosition(globalPosition);
				shape.setRotation(globalRotation);
				shape.setScale(globalScale * glm::vec3(getSize().value(), 1.0f));
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
		void GUIShape::addToView()
		{
			enableBit(updateFlags, GUIElementUpdateFlag::PrimitiveAddNeeded);
			GUIElement::addToView();
		}

		bool GUIShape::hitTest(const glm::vec2& _viewPoint)
		{
			return false;
		}


		ShapeType GUIShape::getShapeType() const
		{
			return shape.getShapeType();
		}
		const Color& GUIShape::getColor() const
		{
			return shape.getColor();
		}
		std::shared_ptr<Material> GUIShape::getMaterial() const
		{
			return shape.getMaterial();
		}
		bool GUIShape::getVisible() const
		{
			return shape.getRenderState();
		}


		void GUIShape::setShapeType(ShapeType _type)
		{
			shape.generate(_type);
		}
		void GUIShape::setColor(const Color& _color)
		{
			shape.setColor(_color);
		}
		void GUIShape::setMaterial(std::shared_ptr<Material> _material)
		{
			shape.setMaterial(_material);
			disableBit(updateFlags, GUIElementUpdateFlag::MaterialUpdateNeeded);
		}
		void GUIShape::setVisible(bool _visible)
		{
			shape.setRenderState(_visible);
		}
	}
}
