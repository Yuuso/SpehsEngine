#include "stdafx.h"
#include "SpehsEngine/GUI/GUIElement.h"

#include "SpehsEngine/Math/GLMMatrixUtilityFunctions.h"
#include "SpehsEngine/Math/Bounds2D.h"


namespace se
{
	namespace gui
	{
		GUIElement::~GUIElement()
		{
			se_assert(!isRootElement);
			if (parent)
				parent->removeChild(this);
			clearChildren();
		}
		GUIElement::GUIElement(const GUIElement& _other)
			: position(_other.position)
			, zindex(_other.zindex)
			, rotation(_other.rotation)
			, size(_other.size)
			, scale(_other.scale)
			, clipping(_other.clipping)
			, anchor(_other.anchor)
			, alignment(_other.alignment)
			, visible(_other.visible)
		{
			parent = nullptr;
			for (auto&& copyChild : _other.children)
				addChild(std::shared_ptr<GUIElement>(copyChild->clone()));
		}

		std::shared_ptr<GUIElement> GUIElement::clone()
		{
			return std::make_shared<GUIElement>(*this);
		}


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

		GUIUnit GUIElement::resolveUnitType(GUIUnit _unit, bool _isWidth)
		{
			GUIUnit result = _unit;
			switch (_unit.type)
			{
				default:
					break;
				case GUIUnitType::Self:
					result.type = _isWidth ? GUIUnitType::SelfWidth : GUIUnitType::SelfHeight;
					break;
				case GUIUnitType::Parent:
					result.type = _isWidth ? GUIUnitType::ParentWidth : GUIUnitType::ParentHeight;
					break;
				case GUIUnitType::View:
					result.type = _isWidth ? GUIUnitType::ViewWidth : GUIUnitType::ViewHeight;
					break;
			}
			return result;
		}
		float GUIElement::unitToPixels(GUIUnit _unit, const glm::vec2& _viewSize) const
		{
			GUIUnitType type = GUIUnitType::Undefined;
			switch (_unit.type)
			{
				default:
					type = _unit.type;
					break;

				case GUIUnitType::Auto:
					// As intended
					type = GUIUnitType::Pixels;
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
					return parent->unitToPixels(resolveUnitType(parent->getSize().x, true), _viewSize);
				}
				return _viewSize.x;
			};
			auto parentH = [&]
			{
				if (parent)
				{
					return parent->unitToPixels(resolveUnitType(parent->getSize().y, false), _viewSize);
				}
				return _viewSize.y;
			};

			switch (type)
			{
				default:
					se_assert_m(false, "Unknown unit type!");
					return _unit.value;

				case GUIUnitType::Pixels:					return _unit.value;

				case se::gui::GUIUnitType::SelfWidth:		return unitToPixels(resolveUnitType(getSize().x, true), _viewSize) * _unit.value;
				case se::gui::GUIUnitType::SelfHeight:		return unitToPixels(resolveUnitType(getSize().y, false), _viewSize) * _unit.value;
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
		glm::vec2 GUIElement::unitToPixels(const GUIVec2& _vec, const glm::vec2& _viewSize) const
		{
			return { unitToPixels(resolveUnitType(_vec.x, true), _viewSize),
					 unitToPixels(resolveUnitType(_vec.y, false), _viewSize) };
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
				const glm::vec2 pixelPosition = unitToPixels(getPosition(), _context.viewSize);
				const glm::vec2 pixelSize = unitToPixels(getSize(), _context.viewSize);

				const glm::mat4 localTransform =
					constructTransformationMatrix(
						glm::vec3(pixelPosition.x + anchorOffset.x + alignmentOffset.x, -(pixelPosition.y + anchorOffset.y + alignmentOffset.y), zValue),
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
			lastViewSize = _context.viewSize;
		}
		void GUIElement::onAddedToView()
		{
			for (auto&& child : children)
				child->onAddedToView();
		}
		void GUIElement::onRemovedFromView()
		{
			for (auto&& child : children)
				child->onRemovedFromView();
		}
		void GUIElement::removeFromView()
		{
			se_assert(isRootElement);
			isRootElement = false;
			onRemovedFromView();
		}
		void GUIElement::addToView()
		{
			se_assert(!isRootElement);
			isRootElement = true;
			onAddedToView();
		}

		size_t GUIElement::addChild(std::shared_ptr<GUIElement> _element)
		{
			se_assert(_element.get() != this);
			if (_element->parent)
			{
				if (_element->parent == this)
				{
					for (size_t i = 0; i < children.size(); i++)
					{
						if (children[i] == _element)
						{
							log::warning("GUIElement::addChild: Element already a child of this element!");
							return i;
						}
					}
					se_assert(false);
					log::warning("GUIElement::addChild: Already the parent of this element, but element not found in children!?!");
				}
				else
				{
					log::warning("GUIElement::addChild: Element already a child of another element!");
					_element->parent->removeChild(_element);
				}
			}
			children.push_back(_element);
			_element->parent = this;
			_element->onAddedParent();
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
			return children.size() - 1;
		}
		void GUIElement::removeChild(size_t _index)
		{
			se_assert(_index < children.size());
			auto child = getChild(_index);
			children.erase(children.begin() + _index);
			child->parent = nullptr;
			child->onRemovedParent();
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::removeChild(GUIElement* _element)
		{
			for (size_t i = 0; i < children.size(); i++)
			{
				if (children[i].get() == _element)
				{
					removeChild(i);
					return;
				}
			}
			log::warning("GUIElement::removeChild: Element not found!");
		}
		void GUIElement::removeChild(std::shared_ptr<GUIElement> _element)
		{
			removeChild(_element.get());
		}
		void GUIElement::clearChildren()
		{
			for (auto&& child : children)
			{
				child->parent = nullptr;
				child->onRemovedParent();
			}
			children.clear();
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		std::shared_ptr<GUIElement> GUIElement::getChild(size_t _index)
		{
			se_assert(_index < children.size());
			if (_index < children.size())
				return children[_index];
			return nullptr;
		}
		size_t GUIElement::getNumChildren() const
		{
			return children.size();
		}

		bool GUIElement::hitTest(const glm::vec2& _viewPoint)
		{
			glm::vec3 globalPosition;
			//glm::quat globalRotation; // TODO
			glm::vec3 globalScale;
			decomposeTransformationMatrix(globalTrasform, &globalPosition, nullptr, &globalScale);

			const glm::vec2 globalSize = unitToPixels(getSize(), lastViewSize) * glm::vec2(globalScale.x, globalScale.y);
			const glm::vec2 pixelOffset = -unitToPixels(getTransformOffset(), lastViewSize);

			Bounds2D bounds(glm::vec2(globalPosition.x + globalSize.x * 0.5f + pixelOffset.x, -globalPosition.y + globalSize.y * 0.5f + pixelOffset.y), globalSize * 0.5f);
			return bounds.contains(_viewPoint);
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
		void GUIElement::setX(GUIUnit _x)
		{
			position.x = _x;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setY(GUIUnit _y)
		{
			position.y = _y;
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
		void GUIElement::setWidth(GUIUnit _width)
		{
			size.x = _width;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setHeight(GUIUnit _height)
		{
			size.y = _height;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setScale(const glm::vec2& _scale)
		{
			scale = _scale;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setClipping(bool _value)
		{
			if (clipping == _value)
				return;
			clipping = _value;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setVisible(bool _value)
		{
			if (visible == _value)
				return;
			visible = _value;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}

		void GUIElement::setAnchor(const GUIVec2& _anchor)
		{
			anchor = _anchor;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setAnchor(VerticalAnchor _vertical, HorizontalAnchor _horizontal)
		{
			anchor = GUIVec2(anchorToUnit(_horizontal), anchorToUnit(_vertical));
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setVerticalAnchor(VerticalAnchor _value)
		{
			anchor.y = anchorToUnit(_value);
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setHorizontalAnchor(HorizontalAnchor _value)
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
