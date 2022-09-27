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

		void GUIText::elementPreUpdate(UpdateContext& _context)
		{
			if (checkBit(updateFlags, GUIElementUpdateFlag::MaterialUpdateNeeded))
			{
				// Material updated here, because it's needed for dimension update
				text.setMaterial(_context.materialManager.createFontMaterial(getFont()));
			}
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
							setWidth(GUIUnit(dims.capsDimensions.x, GUIUnitType::Auto));
						}
						else
						{
							const float factor = unitToPixels(resolveUnitType(getSize().y, false), _context.viewSize) / dims.capsDimensions.y;
							setWidth(GUIUnit(dims.capsDimensions.x * factor, GUIUnitType::Auto));
						}
					}
					if (getSize().y.type == GUIUnitType::Auto)
					{
						if (getSize().x.type == GUIUnitType::Auto)
						{
							setHeight(GUIUnit(dims.capsDimensions.y, GUIUnitType::Auto));
						}
						else
						{
							const float factor = unitToPixels(resolveUnitType(getSize().x, true), _context.viewSize) / dims.capsDimensions.x;
							setHeight(GUIUnit(dims.capsDimensions.y * factor, GUIUnitType::Auto));
						}
					}
				}
			}
		}
		void GUIText::elementUpdate(UpdateContext& _context)
		{
			// NOTE: MaterialUpdateNeeded handled in elementPreUpdate
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
			if (checkBit(updateFlags, GUIElementUpdateFlag::VisualUpdateNeeded))
			{
				text.setColor(getColor());
				se_assert(text.getMaterial());
				text.getMaterial()->setFont(_context.materialManager.getFont(getFont()));
			}
		}

		glm::vec3 GUIText::getRenderScale(const glm::vec2& _viewSize)
		{
			if (needUpdateDimensions)
				return glm::vec3(1.0f);
			const TextDimensions& dims = text.getDimensions();
			return glm::vec3(unitToPixels(getSize(), _viewSize) / dims.capsDimensions, 1.0f);
		}

		const Color& GUIText::getColor() const
		{
			return_property(color);
		}
		std::string_view GUIText::getFont() const
		{
			return_property(font);
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
			normalProperties.color = _color;
			enableBit(updateFlags, GUIElementUpdateFlag::VisualUpdateNeeded);
		}
		void GUIText::setFont(std::string_view _font)
		{
			normalProperties.font = _font;
			if (!text.getMaterial())
			{
				enableBit(updateFlags, GUIElementUpdateFlag::MaterialUpdateNeeded);
			}
			else
			{
				enableBit(updateFlags, GUIElementUpdateFlag::VisualUpdateNeeded);
			}
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
