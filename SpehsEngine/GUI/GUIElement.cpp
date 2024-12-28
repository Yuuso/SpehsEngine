#include "stdafx.h"
#include "SpehsEngine/GUI/GUIElement.h"

#include "SpehsEngine/GUI/Internal/StencilMaskManager.h"
#include "SpehsEngine/Input/MouseUtilityFunctions.h"
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
			: normalProperties(_other.normalProperties)
			, hoverProperties(_other.hoverProperties)
			, pressedProperties(_other.pressedProperties)
			, consumeInput(_other.consumeInput)
			, inheritInputStatus(_other.inheritInputStatus)
			, updateFlags(GUIElementUpdateFlag::EverythingChanged)
		{
			// NOTE: dataContext and callbacks are not copied currently!
			for (auto&& copyChild : _other.children)
				addChild(copyChild->clone());
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
		bool GUIElement::needInputUpdate() const
		{
			return hoverCallback
				|| clickCallback
				|| hoverProperties.has_value()
				|| pressedProperties.has_value();
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
					return parent->unitToPixels(resolveUnitType(parent->getSize().x, true), _viewSize) -
						   parent->unitToPixels(resolveUnitType(parent->getPadding(), true), _viewSize) * 2.0f;
				}
				return _viewSize.x;
			};
			auto parentH = [&]
			{
				if (parent)
				{
					return parent->unitToPixels(resolveUnitType(parent->getSize().y, false), _viewSize) -
						   parent->unitToPixels(resolveUnitType(parent->getPadding(), false), _viewSize) * 2.0f;
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

		void GUIElement::preUpdate(UpdateContext& _context)
		{
			if (preUpdateCallback)
				preUpdateCallback(*this);

			if (parent && checkBit(parent->updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded))
				enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
			if (lastViewSize != _context.viewSize)
				enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);

			// Hover input update (inputUpdate currently only is called on mouse button presses)
			GUIElementInputStatus lastStatus = inputStatus;
			const ZIndex zvalue = getZValue();
			bool waitingToHoverUpdate = false;
			bool inheritStatus = inheritInputStatus && parent && parent->inputStatus == GUIElementInputStatus::Hover;

			{
				if (getVisible() && needInputUpdate())
				{
					if (const std::optional<glm::vec2> mousePosition = se::input::getMousePositionf())
					{
						const bool notConsumed = _context.hoverHandledDepth <= zvalue;
						if ((notConsumed && hitTest(*mousePosition)) || inheritStatus)
						{
							if (notConsumed && consumeInput)
								_context.hoverHandledDepth = zvalue;
							waitingToHoverUpdate = true;
							if (inputStatus == GUIElementInputStatus::Normal)
							{
								inputStatus = GUIElementInputStatus::Hover;
							}
						}
					}
				}
			}

			elementPreUpdate(_context);
			for (size_t i = 0; i < children.size(); i++)
			{
				children[i]->preUpdate(_context);
			}

			if (waitingToHoverUpdate && ((_context.hoverHandledDepth <= zvalue) || inheritStatus))
			{
				if (inputStatus == GUIElementInputStatus::Hover)
				{
					if (hoverCallback)
						hoverCallback(*this);
				}
			}
			else
			{
				inputStatus = GUIElementInputStatus::Normal;
			}

			if (lastStatus != inputStatus)
			{
				enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
				enableBit(updateFlags, GUIElementUpdateFlag::VisualUpdateNeeded);
			}
		}
		void GUIElement::update(UpdateContext& _context)
		{
			if (parent && checkBit(parent->updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded))
				enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
			if (lastViewSize != _context.viewSize)
				enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);

			if (checkBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded))
			{
				const float zValue = static_cast<float>(getZValue());
				const glm::vec2 anchorOffset = getAnchorPositionOffset(_context.viewSize);
				const glm::vec2 alignmentOffset = getAlignmentPositionOffset(_context.viewSize);
				const glm::vec2 pixelPosition = unitToPixels(getPosition(), _context.viewSize);
				const glm::vec2 pixelSize = unitToPixels(getSize(), _context.viewSize);
				const float paddingOffset = parent ? parent->unitToPixels(parent->getPadding(), _context.viewSize) : 0.0f;

				const glm::mat4 localTransform =
					constructTransformationMatrix(
						glm::vec3(pixelPosition.x + anchorOffset.x + alignmentOffset.x + paddingOffset, -(pixelPosition.y + anchorOffset.y + alignmentOffset.y + paddingOffset), zValue),
						glm::rotate(glm::identity<glm::quat>(), getRotation(), glm::vec3(0.0f, 0.0f, 1.0f)),
						glm::vec3(getScale().x, getScale().y, 1.0f));
				if (parent)
				{
					globalTrasform = parent->globalTrasform * localTransform;
				}
				else
				{
					globalTrasform = localTransform;
				}

				globalVisible = getVisible() && (parent ? parent->globalVisible : true);

				globalScissor.enabled = false;
				if (parent && parent->globalScissor.enabled)
				{
					globalScissor = parent->globalScissor;
				}
				else if (getScissorMask())
				{
					// Reset
					globalScissor.x = 0;
					globalScissor.y = 0;
					globalScissor.width = static_cast<uint16_t>(_context.viewSize.x);
					globalScissor.height = static_cast<uint16_t>(_context.viewSize.y);
				}
				if (getScissorMask())
				{
					se_assert_m(getRotation() == 0.0f, "GUIElement: Rotation not supported with ScissorMask!");
					glm::vec3 globalPosition;
					glm::vec3 globalScale;
					decomposeTransformationMatrix(globalTrasform, &globalPosition, nullptr, &globalScale);

					const glm::vec2 pixelOffset = unitToPixels(getTransformOffset(), _context.viewSize);
					globalPosition.x -= pixelOffset.x; // don't ask why this is -= instead of += ...
					globalPosition.y += pixelOffset.y;

					globalScissor.enabled = true;
					if (globalPosition.x >= 0.0f)
					{
						globalScissor.x = static_cast<uint16_t>(rintf(globalPosition.x));
						globalScissor.width = static_cast<uint16_t>(rintf(pixelSize.x * globalScale.x));
					}
					else
					{
						globalScissor.x = 0;
						globalScissor.width = static_cast<uint16_t>(std::max(0.0f, rintf(pixelSize.x * globalScale.x + globalPosition.x)));
					}
					if (globalPosition.y < 0.0f)
					{
						globalScissor.y = static_cast<uint16_t>(rintf(-globalPosition.y));
						globalScissor.height = static_cast<uint16_t>(rintf(pixelSize.y * globalScale.y));
					}
					else
					{
						globalScissor.y = 0;
						globalScissor.height = static_cast<uint16_t>(std::max(0.0f, rintf(pixelSize.y * globalScale.y - globalPosition.y)));
					}
				}
			}

			if (getLayerMask())
			{
				_context.stencilMaskManager.begin();
			}

			elementUpdate(_context);
			for (auto&& child : children)
				child->update(_context);

			if (getLayerMask())
			{
				_context.stencilMaskManager.end();
			}

			updateFlags = 0;
			lastViewSize = _context.viewSize;
		}
		bool GUIElement::inputUpdate(InputUpdateContext& _context)
		{
			GUIElementInputStatus lastStatus = inputStatus;
			const ZIndex zvalue = getZValue();
			bool waitingToUpdate = false;
			bool inputHandled = false;
			const bool inheritStatus = inheritInputStatus && parent && parent->inputStatus != GUIElementInputStatus::Normal;

			if (_context.mouseButtonEvent.button == input::MouseButton::left)
			{
				if (getVisible() && needInputUpdate())
				{
					if (const std::optional<glm::vec2> mousePosition = se::input::getMousePositionf())
					{
						const bool notConsumed = _context.handledDepth <= zvalue;
						if ((notConsumed && hitTest(*mousePosition)) || inheritStatus)
						{
							// Mark our depth, but wait for children to check before actually handling the event
							if (notConsumed && consumeInput)
								_context.handledDepth = zvalue;
							waitingToUpdate = true;
							if (inputStatus == GUIElementInputStatus::Normal)
							{
								inputStatus = GUIElementInputStatus::Hover;
							}
						}
					}
				}
			}

			for (size_t i = 0; i < children.size(); i++)
				inputHandled = children[i]->inputUpdate(_context) || inputHandled;

			if (waitingToUpdate && ((_context.handledDepth <= zvalue) || inheritStatus))
			{
				if (_context.mouseButtonEvent.isPress())
				{
					inputStatus = GUIElementInputStatus::Pressed;
				}
				else if (_context.mouseButtonEvent.isHold() && inputStatus == GUIElementInputStatus::Pressed)
				{
					inputStatus = GUIElementInputStatus::Pressed;
				}
				else
				{
					inputStatus = GUIElementInputStatus::Hover;
				}

				if (_context.mouseButtonEvent.isRelease() && clickCallback)
					clickCallback(*this);
				if (hoverCallback)
					hoverCallback(*this);
				inputHandled = true;
			}
			else
			{
				inputStatus = GUIElementInputStatus::Normal;
			}

			if (lastStatus != inputStatus)
			{
				enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
				enableBit(updateFlags, GUIElementUpdateFlag::VisualUpdateNeeded);
			}

			return inputHandled;
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
		bool GUIElement::removeChild(std::string_view _name)
		{
			for (size_t i = 0; i < children.size(); i++)
			{
				if (children[i]->name == _name)
				{
					removeChild(i);
					return true;
				}
			}
			return false;
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
		const GUIElement* GUIElement::getChild(size_t _index) const
		{
			se_assert(_index < children.size());
			if (_index < children.size())
				return children[_index].get();
			return nullptr;
		}
		std::shared_ptr<GUIElement> GUIElement::findChild(std::string_view _name, bool _recursive)
		{
			for (auto&& child : children)
			{
				if (child->getName() == _name)
					return child;
			}
			if (_recursive)
			{
				for (auto&& child : children)
				{
					if (auto result = child->findChild(_name, true))
						return result;
				}
			}
			return nullptr;
		}
		size_t GUIElement::getNumChildren() const
		{
			return children.size();
		}
		GUIElement* GUIElement::getParent()
		{
			return parent;
		}
		const GUIElement* GUIElement::getParent() const
		{
			return parent;
		}

		void GUIElement::setUpdateCallback(std::function<void(GUIElement&)> _callback)
		{
			preUpdateCallback = _callback;
		}
		bool GUIElement::hitTest(const glm::vec2& _viewPoint)
		{
			glm::vec3 globalPosition;
			//glm::quat globalRotation; // TODO
			glm::vec3 globalScale;
			decomposeTransformationMatrix(globalTrasform, &globalPosition, nullptr, &globalScale);

			const glm::vec2 scale{ globalScale.x, globalScale.y };
			const glm::vec2 globalSize = unitToPixels(getSize(), lastViewSize) * scale;
			const glm::vec2 pixelOffset = -unitToPixels(getTransformOffset(), lastViewSize) * scale;

			const Bounds2D bounds(glm::vec2(globalPosition.x + globalSize.x * 0.5f + pixelOffset.x, -globalPosition.y + globalSize.y * 0.5f + pixelOffset.y), globalSize * 0.5f);
			return bounds.contains(_viewPoint);
		}
		void GUIElement::onClick(std::function<void(GUIElement&)> _callback)
		{
			clickCallback = _callback;
		}
		void GUIElement::onHover(std::function<void(GUIElement&)> _callback)
		{
			hoverCallback = _callback;
		}
		void GUIElement::setHoverProperties(const GUIElementProperties& _properties)
		{
			hoverProperties = _properties;
		}
		void GUIElement::setPressedProperties(const GUIElementProperties& _properties)
		{
			pressedProperties = _properties;
		}
		void GUIElement::setInheritInputStatus(bool _value)
		{
			inheritInputStatus = _value;
		}

		const std::string& GUIElement::getName() const
		{
			return name;
		}
		const GUIVec2& GUIElement::getPosition() const
		{
			return_property(position);
		}
		ZIndex GUIElement::getZIndex() const
		{
			return_property(zindex);
		}
		ZIndex GUIElement::getZValue() const
		{
			const ZIndex parentZValue = parent ? parent->getZValue() : 0;
			return parentZValue + 1 + getZIndex();
		}
		float GUIElement::getRotation() const
		{
			return_property(rotation);
		}
		const GUIVec2& GUIElement::getSize() const
		{
			return_property(size);
		}
		const glm::vec2& GUIElement::getScale() const
		{
			return_property(scale);
		}
		bool GUIElement::getScissorMask() const
		{
			return_property(scissorMask);
		}
		bool GUIElement::getLayerMask() const
		{
			return_property(layerMask);
		}
		const GUIVec2& GUIElement::getAnchor() const
		{
			return_property(anchor);
		}
		const GUIVec2& GUIElement::getAlignment() const
		{
			return_property(alignment);
		}
		bool GUIElement::getVisible() const
		{
			return_property(visible);
		}
		GUIUnit GUIElement::getPadding() const
		{
			return_property(padding);
		}
		bool GUIElement::getConsumeInput() const
		{
			return consumeInput;
		}


		void GUIElement::setName(std::string_view _name)
		{
			name = _name;
		}
		void GUIElement::setPosition(const GUIVec2& _position)
		{
			normalProperties.position = _position;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setX(GUIUnit _x)
		{
			normalProperties.position->x = _x;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setY(GUIUnit _y)
		{
			normalProperties.position->y = _y;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setZIndex(ZIndex _zindex)
		{
			normalProperties.zindex = _zindex;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setRotation(float _radians)
		{
			normalProperties.rotation = _radians;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setSize(const GUIVec2& _size)
		{
			normalProperties.size = _size;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setWidth(GUIUnit _width)
		{
			normalProperties.size->x = _width;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setHeight(GUIUnit _height)
		{
			normalProperties.size->y = _height;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setScale(const glm::vec2& _scale)
		{
			normalProperties.scale = _scale;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setScale(float _scale)
		{
			setScale(glm::vec2(_scale, _scale));
		}
		void GUIElement::setScissorMask(bool _value)
		{
			if (normalProperties.scissorMask == _value)
				return;
			normalProperties.scissorMask = _value;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setVisible(bool _value)
		{
			if (normalProperties.visible == _value)
				return;
			normalProperties.visible = _value;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}

		void GUIElement::setAnchor(const GUIVec2& _anchor)
		{
			normalProperties.anchor = _anchor;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setAnchor(VerticalAnchor _vertical, HorizontalAnchor _horizontal)
		{
			normalProperties.anchor = GUIVec2(anchorToUnit(_horizontal), anchorToUnit(_vertical));
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setVerticalAnchor(VerticalAnchor _value)
		{
			normalProperties.anchor->y = anchorToUnit(_value);
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setHorizontalAnchor(HorizontalAnchor _value)
		{
			normalProperties.anchor->x = anchorToUnit(_value);
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}

		void GUIElement::setAlignment(const GUIVec2& _alignment)
		{
			normalProperties.alignment = _alignment;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setAlignment(VerticalAlignment _vertical, HorizontalAlignment _horizontal)
		{
			normalProperties.alignment = GUIVec2(alignmentToUnit(_horizontal), alignmentToUnit(_vertical));
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setVerticalAlignment(VerticalAlignment _value)
		{
			normalProperties.alignment->y = alignmentToUnit(_value);
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setHorizontalAlignment(HorizontalAlignment _value)
		{
			normalProperties.alignment->x = alignmentToUnit(_value);
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setPadding(GUIUnit _padding)
		{
			normalProperties.padding = _padding;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}
		void GUIElement::setConsumeInput(bool _value)
		{
			consumeInput = _value;
		}

		void GUIElement::setDataContext(std::any _context)
		{
			dataContext = _context;
		}
	}
}
