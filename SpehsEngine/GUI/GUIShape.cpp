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
		{
			initShape();
			setColor(_other.getColor());
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
				shape.setMaterial(_context.materialManager.createShapeMaterial(getTexture()));
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

					if (getColor().a < 1.0f || !getTexture().empty())
						shape.setRenderFlags(RenderFlag::BlendAlpha | RenderFlag::DepthTestLess);
					else
						shape.setRenderFlags(RenderFlag::WriteDepth | RenderFlag::WriteAlpha | RenderFlag::DepthTestLess);
				}
			}
			if (checkBit(updateFlags, GUIElementUpdateFlag::VisualUpdateNeeded))
			{
				shape.setColor(getColor());
				shape.getMaterial()->setTexture(_context.materialManager.getTexture(getTexture()));
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

		const Color& GUIShape::getColor() const
		{
			return_property(color);
		}
		const std::string_view GUIShape::getTexture() const
		{
			return_property(texture);
		}

		void GUIShape::setColor(const Color& _color)
		{
			normalProperties.color = _color;
			enableBit(updateFlags, GUIElementUpdateFlag::VisualUpdateNeeded);
		}
		void GUIShape::setTexture(std::string_view _name)
		{
			normalProperties.texture = _name;

			if (!shape.getMaterial())
			{
				enableBit(updateFlags, GUIElementUpdateFlag::MaterialUpdateNeeded);
			}
			else
			{
				enableBit(updateFlags, GUIElementUpdateFlag::VisualUpdateNeeded);
			}
		}
	}
}
