#include "stdafx.h"
#include "SpehsEngine/GUI/GUIText.h"

#include "SpehsEngine/GUI/GUIView.h"
#include "SpehsEngine/Math/GLMMatrixUtilityFunctions.h"


namespace se
{
	using namespace graphics;

	namespace gui
	{
		GUIText::GUIText()
		{
			text.setOrientation(ShapeOrientation::XY_Plane);
			enableBit(updateFlags, GUIElementUpdateFlag::MaterialUpdateNeeded);
		}


		void GUIText::elementUpdate(UpdateContext& _context)
		{
			if (checkBit(updateFlags, GUIElementUpdateFlag::MaterialUpdateNeeded))
			{
				// TODO
				se_assert(false);
			}
			if (checkBit(updateFlags, GUIElementUpdateFlag::PrimitiveAddNeeded))
			{
				_context.view.getView().getScene().add(text);
			}
			if (checkBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded))
			{
				text.setRenderState(globalVisible);
				if (globalVisible)
				{
					glm::vec3 globalPosition;
					glm::quat globalRotation;
					glm::vec3 globalScale;
					decomposeTransformationMatrix(globalTrasform, &globalPosition, &globalRotation, &globalScale);

					text.setPosition(globalPosition);
					text.setRotation(globalRotation);
					text.setScale(globalScale);

					text.setScissor(globalScissor);
				}
			}
		}


		const Color& GUIText::getColor() const
		{
			return text.getColor();
		}
		std::shared_ptr<graphics::Material> GUIText::getMaterial() const
		{
			return text.getMaterial();
		}

		const size_t GUIText::length() const
		{
			return text.length();
		}
		std::string GUIText::getPlainText() const
		{
			return text.getPlainText();
		}
		const size_t GUIText::getPenPosition() const
		{
			return text.getPenPosition();
		}
		const float	 GUIText::getLineSpacing() const
		{
			return text.getLineSpacing();
		}


		void GUIText::setColor(const Color& _color)
		{
			text.setColor(_color);
		}
		void GUIText::setMaterial(std::shared_ptr<graphics::Material> _material)
		{
			text.setMaterial(_material);
			disableBit(updateFlags, GUIElementUpdateFlag::MaterialUpdateNeeded);
		}

		void GUIText::setLineSpacing(const float _amount)
		{
			text.setLineSpacing(_amount);
			updateDimensions();
		}
		void GUIText::movePen(const int _movement)
		{
			text.movePen(_movement);
		}

		void GUIText::insert(const std::string& _text)
		{
			text.insert(_text);
			updateDimensions();
		}
		void GUIText::clear()
		{
			text.clear();
			updateDimensions();
		}

		void GUIText::updateDimensions()
		{
			// TODO: performance?
			const TextDimensions& dims = text.getDimensions();
			setSize(dims.dimensions);
		}

		void GUIText::onAddedParent()
		{
			enableBit(updateFlags, GUIElementUpdateFlag::PrimitiveAddNeeded);
			GUIElement::onAddedParent();
		}
		void GUIText::onRemovedParent()
		{
			text.removeFromScenes();
			GUIElement::onRemovedParent();
		}
		void GUIText::onRemovedFromView()
		{
			text.removeFromScenes();
			GUIElement::onRemovedFromView();
		}
		void GUIText::addToView()
		{
			enableBit(updateFlags, GUIElementUpdateFlag::PrimitiveAddNeeded);
			GUIElement::addToView();
		}
		glm::vec2 GUIText::getTransformOffset()
		{
			return { -text.getDimensions().offsetFromOrigin.x, text.getDimensions().offsetFromOrigin.y };
		}
	}
}
