#include "stdafx.h"
#include "SpehsEngine/GUI/GUIElement.h"

#include "SpehsEngine/Math/GLMMatrixUtilityFunctions.h"


namespace se
{
	namespace gui
	{
		glm::vec2 GUIElement::getAnchorPositionOffset()
		{
			// Assuming default is center!
			// TODO: assuming self unitype
			return {(getSize().x * 0.5f) - (getSize().x * getAnchor().x),
					(getSize().y * 0.5f) - (getSize().y * getAnchor().y) };
		}
		glm::vec2 GUIElement::getAlignmentPositionOffset()
		{
			// TODO: assuming parent unitype
			if (parent)
			{
				// Assuming default is center middle!
				return {(-parent->getSize().x * 0.5f) + (parent->getSize().x * getAlignment().x),
						(-parent->getSize().y * 0.5f) + (parent->getSize().y * getAlignment().y) };
			}
			else
			{
				// Default is top left
				// TODO
				return {};
			}
		}

		void GUIElement::update(UpdateContext& _context)
		{
			if (parent && checkBit(parent->updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded))
				enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);

			if (checkBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded))
			{
				const float zValue = static_cast<float>(1 + getZIndex());

				const glm::vec2 anchorOffset = getAnchorPositionOffset();
				const glm::vec2 alignmentOffset = getAlignmentPositionOffset();

				const glm::mat4 localTransform =
					constructTransformationMatrix(
						glm::vec3(getPosition().x + anchorOffset.x + alignmentOffset.x, -(getPosition().y + anchorOffset.y + alignmentOffset.y), zValue),
						glm::rotate(glm::identity<glm::quat>(), getRotation(), glm::vec3(0.0f, 0.0f, 1.0f)),
						glm::vec3(getScale().x, getScale().y, 1.0f));
				if (parent)
					globalTrasform = parent->globalTrasform * localTransform;
				else
					globalTrasform = localTransform;
			}

			elementUpdate(_context);
			for (auto&& child : children)
				child->update(_context);

			updateFlags = 0;
		}
		void GUIElement::onAddedParent()
		{
			se_assert(parent);
		}
		void GUIElement::onRemovedParent()
		{
			se_assert(!parent);
		}
		void GUIElement::removeFromView()
		{
			onRemovedFromView();
			for (auto&& child : children)
				child->removeFromView();
		}

		void GUIElement::addChild(GUIElement& _element)
		{
			se_assert(&_element != this);
			if (_element.parent)
			{
				if (_element.parent == this)
				{
					log::warning("GUIElement::addChild: Element already a child of this element!");
					return;
				}

				log::warning("GUIElement::addChild: Element already a child of another element!");
				_element.parent->removeChild(_element);
			}
			children.push_back(&_element);
			_element.parent = this;
			_element.onAddedParent();
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::removeChild(GUIElement& _element)
		{
			auto it = std::find(children.begin(), children.end(), &_element);
			if (it == children.end())
			{
				log::warning("GUIElement::removeChild: Element not found!");
				return;
			}
			children.erase(it);
			_element.parent = nullptr;
			_element.onRemovedParent();
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}

		bool GUIElement::hitTest(const glm::vec2& _viewPoint)
		{
			return false;
		}


		const GUIVec2& GUIElement::getPosition() const
		{
			return position;
		}
		ZIndex GUIElement::getZIndex() const
		{
			return zindex;
		}
		float GUIElement::getRotation() const
		{
			return rotation;
		}
		const GUIVec2& GUIElement::getSize() const
		{
			return size;
		}
		const glm::vec2& GUIElement::getScale() const
		{
			return scale;
		}
		bool GUIElement::getClipping() const
		{
			return clipping;
		}
		const Margin& GUIElement::getMargin() const
		{
			return margin;
		}
		const Padding& GUIElement::getPadding() const
		{
			return padding;
		}
		const GUIVec2& GUIElement::getAnchor() const
		{
			return anchor;
		}
		const GUIVec2& GUIElement::getAlignment() const
		{
			return alignment;
		}


		void GUIElement::setPosition(const GUIVec2& _position)
		{
			position = _position;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setZIndex(ZIndex _zindex)
		{
			zindex = _zindex;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setRotation(float _radians)
		{
			rotation = _radians;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setSize(const GUIVec2& _size)
		{
			size = _size;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setScale(const glm::vec2& _scale)
		{
			scale = _scale;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setClipping(bool _value)
		{
			clipping = _value;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setMargin(const Margin& _margin)
		{
			margin = _margin;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setPadding(const Padding& _padding)
		{
			padding = _padding;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}

		void GUIElement::setAnchor(const GUIVec2& _anchor)
		{
			anchor = _anchor;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setAnchor(VerticalAlignment _vertical, HorizontalAlignment _horizontal)
		{
			anchor = GUIVec2(alignmentToUnit(_horizontal), alignmentToUnit(_vertical));
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setVerticalAnchor(VerticalAlignment _value)
		{
			anchor.y = alignmentToUnit(_value);
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setHorizontalAnchor(HorizontalAlignment _value)
		{
			anchor.x = alignmentToUnit(_value);
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}

		void GUIElement::setAlignment(const GUIVec2& _alignment)
		{
			alignment = _alignment;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setAlignment(VerticalAlignment _vertical, HorizontalAlignment _horizontal)
		{
			alignment = GUIVec2(alignmentToUnit(_horizontal), alignmentToUnit(_vertical));
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setVerticalAlignment(VerticalAlignment _value)
		{
			alignment.y = alignmentToUnit(_value);
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setHorizontalAlignment(HorizontalAlignment _value)
		{
			alignment.x = alignmentToUnit(_value);
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
	}
}
