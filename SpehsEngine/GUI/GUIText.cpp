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
			initText();
			setSize(GUIVec2(GUIUnitType::Auto));
		}
		GUIText::GUIText(const GUIText& _other)
			: GUIElement(_other)
			, fontName(_other.fontName)
			, needUpdateDimensions(_other.needUpdateDimensions)
		{
			initText();
			setColor(_other.getColor());
			setLineSpacing(_other.getLineSpacing());
			text.insert(_other.text);
		}

		std::shared_ptr<GUIElement> GUIText::clone()
		{
			return std::make_shared<GUIText>(*this);
		}

		void GUIText::initText()
		{
			text.setOrientation(ShapeOrientation::XY_Plane);
			enableBit(updateFlags, GUIElementUpdateFlag::MaterialUpdateNeeded);
		}

		void GUIText::elementPreUpdate()
		{
			if (needUpdateDimensions)
			{
				if (updateDimensions())
				{
					enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
					const TextDimensions& dims = text.getDimensions();
					if (getSize().x.type == GUIUnitType::Auto)
					{
						if (getSize().y.type == GUIUnitType::Auto)
						{
							setWidth(GUIUnit(dims.dimensions.x, GUIUnitType::Auto));
						}
						else
						{
							const float factor = unitToPixels(resolveUnitType(getSize().y, false), lastViewSize) / dims.dimensions.y;
							setWidth(GUIUnit(dims.dimensions.x * factor, GUIUnitType::Auto));
						}
					}
					if (getSize().y.type == GUIUnitType::Auto)
					{
						if (getSize().x.type == GUIUnitType::Auto)
						{
							setHeight(GUIUnit(dims.dimensions.y, GUIUnitType::Auto));
						}
						else
						{
							const float factor = unitToPixels(resolveUnitType(getSize().x, true), lastViewSize) / dims.dimensions.x;
							setHeight(GUIUnit(dims.dimensions.y * factor, GUIUnitType::Auto));
						}
					}
				}
			}
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

					const glm::vec3 renderScale = getRenderScale(_context.viewSize) * globalScale;

					text.setPosition(globalPosition);
					text.setRotation(globalRotation);
					text.setScale(renderScale);

					text.setScissor(globalScissor);
				}
			}
		}

		glm::vec3 GUIText::getRenderScale(const glm::vec2& _viewSize)
		{
			if (needUpdateDimensions)
				return glm::vec3(1.0f);
			const TextDimensions& dims = text.getDimensions();
			return glm::vec3(unitToPixels(getSize(), _viewSize) / dims.dimensions, 1.0f);
		}

		const Color& GUIText::getColor() const
		{
			return text.getColor();
		}

		const size_t GUIText::getTextLength() const
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
			needUpdateDimensions = true;
		}
		void GUIText::movePen(const int _movement)
		{
			text.movePen(_movement);
		}

		void GUIText::insert(const std::string& _text)
		{
			text.insert(_text);
			needUpdateDimensions = true;
		}
		void GUIText::clear()
		{
			text.clear();
			needUpdateDimensions = true;
		}

		bool GUIText::updateDimensions()
		{
			auto material = text.getMaterial();
			if (!material || !material->getFont() || !material->getFont()->ready())
			{
				needUpdateDimensions = true;
				return false;
			}
			needUpdateDimensions = false;
			return true;
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
		void GUIText::onAddedToView()
		{
			enableBit(updateFlags, GUIElementUpdateFlag::PrimitiveAddNeeded);
			GUIElement::onAddedToView();
		}
		GUIVec2 GUIText::getTransformOffset()
		{
			const glm::vec3 renderScale = getRenderScale(lastViewSize);
			return GUIVec2(GUIUnit(text.getDimensions().offsetFromOrigin.x * renderScale.x, GUIUnitType::Pixels), GUIUnit(text.getDimensions().offsetFromOrigin.y * renderScale.y, GUIUnitType::Pixels));
		}
	}
}
