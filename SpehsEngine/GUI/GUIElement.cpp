#include "stdafx.h"
#include "SpehsEngine/GUI/GUIElement.h"

#include "SpehsEngine/Math/GLMMatrixUtilityFunctions.h"


namespace se
{
	namespace gui
	{
		glm::vec2 GUIElement::getAnchorPositionOffset(const glm::vec2& _viewSize)
		{
			const glm::vec2 pixelOffset = unitToPixels(getTransformOffset(), _viewSize);
			const glm::vec2 pixelAnchor = unitToPixels(getAnchor(), _viewSize);
			return { pixelOffset.x - pixelAnchor.x, pixelOffset.y - pixelAnchor.y };
		}
		glm::vec2 GUIElement::getAlignmentPositionOffset(const glm::vec2& _viewSize)
		{
			const glm::vec2 pixelOffset = parent ? -parent->unitToPixels(parent->getTransformOffset(), _viewSize) : glm::vec2();
			const glm::vec2 pixelAlignment = unitToPixels(getAlignment(), _viewSize);
			return { pixelOffset.x + pixelAlignment.x, pixelOffset.y + pixelAlignment.y };
		}
		glm::vec2 GUIElement::getMarginPaddingPositionOffset(const glm::vec2& _viewSize)
		{
			// TODO: all kinds of stuff...
			return unitToPixels(GUIVec2(margin.left, margin.top), _viewSize) + (parent ? parent->unitToPixels(GUIVec2(parent->padding.left, parent->padding.top), _viewSize) : glm::vec2());
		}

		float GUIElement::unitToPixels(GUIUnit _unit, const glm::vec2& _viewSize)
		{
			GUIUnitType type = GUIUnitType::Undefined;
			switch (_unit.type)
			{
				default:
					type = _unit.type;
					break;

				case GUIUnitType::Undefined:
					log::warning("Undefined GUIUnitType (assuming pixels)");
					type = GUIUnitType::Pixels;
					break;

				case se::gui::GUIUnitType::Self:
					se_assert_m(false, "GUIUnitType Self width/height not specified! (assuming width)");
					type = GUIUnitType::SelfWidth;
					break;

				case se::gui::GUIUnitType::Parent:
					se_assert_m(false, "GUIUnitType Parent width/height not specified! (assuming width)");
					type = GUIUnitType::ParentWidth;
					break;

				case se::gui::GUIUnitType::View:
					se_assert_m(false, "GUIUnitType View width/height not specified! (assuming width)");
					type = GUIUnitType::ViewWidth;
					break;
			}

			auto parentW = [&]
			{
				if (parent)
				{
					auto result = parent->unitToPixels(parent->getSize(), _viewSize);
					return result.x;
				}
				return _viewSize.x;
			};
			auto parentH = [&]
			{
				if (parent)
				{
					auto result = parent->unitToPixels(parent->getSize(), _viewSize);
					return result.y;
				}
				return _viewSize.y;
			};

			switch (type)
			{
				default:
					se_assert_m(false, "Unknown unit type!");
					return _unit.value;

				case GUIUnitType::Pixels:					return _unit.value;

				case se::gui::GUIUnitType::SelfWidth:		return unitToPixels(getSize(), _viewSize).x * _unit.value;
				case se::gui::GUIUnitType::SelfHeight:		return unitToPixels(getSize(), _viewSize).y * _unit.value;
				case se::gui::GUIUnitType::SelfMin:			return glm::min(unitToPixels(getSize(), _viewSize).x, unitToPixels(getSize(), _viewSize).y) * _unit.value;
				case se::gui::GUIUnitType::SelfMax:			return glm::max(unitToPixels(getSize(), _viewSize).x, unitToPixels(getSize(), _viewSize).y) * _unit.value;

				case se::gui::GUIUnitType::ParentWidth:		return parentW() * _unit.value;
				case se::gui::GUIUnitType::ParentHeight:	return parentH() * _unit.value;
				case se::gui::GUIUnitType::ParentMin:		return glm::min(parentW(), parentH()) * _unit.value;
				case se::gui::GUIUnitType::ParentMax:		return glm::max(parentW(), parentH()) * _unit.value;

				case se::gui::GUIUnitType::ViewWidth:		return _viewSize.x * _unit.value;
				case se::gui::GUIUnitType::ViewHeight:		return _viewSize.y * _unit.value;
				case se::gui::GUIUnitType::ViewMin:			return glm::min(_viewSize.x, _viewSize.y) * _unit.value;
				case se::gui::GUIUnitType::ViewMax:			return glm::max(_viewSize.x, _viewSize.y) * _unit.value;
			}
		}
		glm::vec2 GUIElement::unitToPixels(const GUIVec2& _vec, const glm::vec2& _viewSize)
		{
			GUIUnitType xType = _vec.x.type;
			switch (xType)
			{
				default:
					break;
				case GUIUnitType::Self:
					xType = GUIUnitType::SelfWidth;
					break;
				case GUIUnitType::Parent:
					xType = GUIUnitType::ParentWidth;
					break;
				case GUIUnitType::View:
					xType = GUIUnitType::ViewWidth;
					break;
			}
			GUIUnitType yType = _vec.y.type;
			switch (yType)
			{
				default:
					break;
				case GUIUnitType::Self:
					yType = GUIUnitType::SelfHeight;
					break;
				case GUIUnitType::Parent:
					yType = GUIUnitType::ParentHeight;
					break;
				case GUIUnitType::View:
					yType = GUIUnitType::ViewHeight;
					break;
			}
			return { unitToPixels(GUIUnit(_vec.x.value, xType), _viewSize), unitToPixels(GUIUnit(_vec.y.value, yType), _viewSize) };
		}

		void GUIElement::preUpdate()
		{
			elementPreUpdate();
			for (auto&& child : children)
				child->preUpdate();
		}
		void GUIElement::update(UpdateContext& _context)
		{
			if (parent && checkBit(parent->updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded))
				enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);

			if (checkBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded))
			{
				const float zValue = static_cast<float>(1 + getZIndex());

				const glm::vec2 anchorOffset = getAnchorPositionOffset(_context.viewSize);
				const glm::vec2 alignmentOffset = getAlignmentPositionOffset(_context.viewSize);
				const glm::vec2 marginPaddingOffset = getMarginPaddingPositionOffset(_context.viewSize);
				const glm::vec2 pixelPosition = unitToPixels(getPosition(), _context.viewSize);
				const glm::vec2 pixelSize = unitToPixels(getSize(), _context.viewSize);

				const glm::mat4 localTransform =
					constructTransformationMatrix(
						glm::vec3(pixelPosition.x + anchorOffset.x + alignmentOffset.x + marginPaddingOffset.x, -(pixelPosition.y + anchorOffset.y + alignmentOffset.y + marginPaddingOffset.y), zValue),
						glm::rotate(glm::identity<glm::quat>(), getRotation(), glm::vec3(0.0f, 0.0f, 1.0f)),
						glm::vec3(getScale().x, getScale().y, 1.0f));
				if (parent)
					globalTrasform = parent->globalTrasform * localTransform;
				else
					globalTrasform = localTransform;

				globalVisible = getVisible() && (parent ? parent->globalVisible : true);

				globalScissor.enabled = false;
				if (clipping)
				{
					glm::vec3 globalPosition;
					glm::vec3 globalScale;
					decomposeTransformationMatrix(globalTrasform, &globalPosition, nullptr, &globalScale);

					globalScissor.enabled = true;
					globalScissor.x = static_cast<uint16_t>(globalPosition.x);
					globalScissor.y = static_cast<uint16_t>(globalPosition.y);
					globalScissor.width = static_cast<uint16_t>(pixelSize.x * globalScale.x);
					globalScissor.height = static_cast<uint16_t>(pixelSize.y * globalScale.y);
				}
				if (parent && parent->globalScissor.enabled)
				{
					if (clipping)
					{
						glm::vec3 globalPosition;
						glm::vec3 globalScale;
						decomposeTransformationMatrix(parent->globalTrasform, &globalPosition, nullptr, &globalScale);

						globalScissor.enabled = true;
						globalScissor.x = glm::max(globalScissor.x, static_cast<uint16_t>(globalPosition.x));
						globalScissor.y = glm::max(globalScissor.y, static_cast<uint16_t>(globalPosition.y));
						globalScissor.width = glm::min(globalScissor.width, static_cast<uint16_t>(pixelSize.x * globalScale.x));
						globalScissor.height = glm::min(globalScissor.height, static_cast<uint16_t>(pixelSize.y * globalScale.y));
					}
					else
					{
						globalScissor = parent->globalScissor;
					}
				}
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
			se_assert(false);
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
		bool GUIElement::getVisible() const
		{
			return visible;
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
		void GUIElement::setVisible(bool _value)
		{
			visible = _value;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}

		void GUIElement::setAnchor(const GUIVec2& _anchor)
		{
			anchor = _anchor;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setAnchor(VerticalAlignment _vertical, HorizontalAlignment _horizontal)
		{
			anchor = GUIVec2(anchorToUnit(_horizontal), anchorToUnit(_vertical));
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setVerticalAnchor(VerticalAlignment _value)
		{
			anchor.y = anchorToUnit(_value);
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setHorizontalAnchor(HorizontalAlignment _value)
		{
			anchor.x = anchorToUnit(_value);
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
