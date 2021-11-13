#include "stdafx.h"
#include "SpehsEngine/GUI/GUIText.h"

#include "SpehsEngine/Graphics/Scene.h"
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

		void GUIText::elementPreUpdate()
		{
			if (needUpdateDimensions)
				updateDimensions();
		}
		void GUIText::elementUpdate(UpdateContext& _context)
		{
			if (checkBit(updateFlags, GUIElementUpdateFlag::MaterialUpdateNeeded))
			{
				text.setMaterial(_context.materialManager.createFontMaterial(fontName));
			}
			if (checkBit(updateFlags, GUIElementUpdateFlag::PrimitiveAddNeeded))
			{
				_context.scene.add(text);
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
		void GUIText::setFont(std::string_view _font)
		{
			fontName = _font;
			enableBit(updateFlags, GUIElementUpdateFlag::MaterialUpdateNeeded);
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
			auto material = text.getMaterial();
			if (!material || !material->getFont() || !material->getFont()->ready())
			{
				needUpdateDimensions = true;
				return;
			}
			const TextDimensions& dims = text.getDimensions();
			setSize(dims.dimensions);
			needUpdateDimensions = false;
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
		GUIVec2 GUIText::getTransformOffset()
		{
			return GUIVec2(GUIUnit(text.getDimensions().offsetFromOrigin.x, GUIUnitType::Pixels), GUIUnit(text.getDimensions().offsetFromOrigin.y, GUIUnitType::Pixels));
		}
	}
}
