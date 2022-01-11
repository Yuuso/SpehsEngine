#include "stdafx.h"
#include "SpehsEngine/GUI/GUIStack.h"


namespace se
{
	namespace gui
	{
		GUIStack::GUIStack(const GUIStack & _other)
			: GUIElement(_other)
			, orientation(_other.orientation)
		{}

		std::shared_ptr<GUIElement> GUIStack::clone()
		{
			return std::make_shared<GUIStack>(*this);
		}


		StackOrientation GUIStack::getOrientation() const
		{
			return orientation;
		}

		void GUIStack::setOrientation(StackOrientation _orientation)
		{
			orientation = _orientation;
			enableBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded);
		}

		void GUIStack::elementPreUpdate(UpdateContext& /*_context*/)
		{
			// TODO: how to optimize this?
			if (getSize().x.type == GUIUnitType::Auto || getSize().y.type == GUIUnitType::Auto)
			{
				glm::vec2 childrenPixelSize;
				for (auto&& child : children)
				{
					if (!child->getVisible())
						continue;
					switch (orientation)
					{
						case StackOrientation::Vertical:
							if (child->getPosition().y.type == GUIUnitType::Auto)
							{
								const glm::vec2 childSize = child->unitToPixels(child->getSize(), lastViewSize);
								childrenPixelSize.y += childSize.y;
								childrenPixelSize.y += unitToPixels(getPadding(), lastViewSize);
								childrenPixelSize.x = glm::max(childrenPixelSize.x, childSize.x);
							}
							break;
						case StackOrientation::Horizontal:
							if (child->getPosition().x.type == GUIUnitType::Auto)
							{
								const glm::vec2 childSize = child->unitToPixels(child->getSize(), lastViewSize);
								childrenPixelSize.x += childSize.x;
								childrenPixelSize.x += unitToPixels(getPadding(), lastViewSize);
								childrenPixelSize.y = glm::max(childrenPixelSize.y, childSize.y);
							}
							break;

						default:
							se_assert(false);
							break;
					}
				}
				if (getSize().x.type == GUIUnitType::Auto)
					setWidth({ childrenPixelSize.x, GUIUnitType::Auto });
				if (getSize().y.type == GUIUnitType::Auto)
					setHeight({ childrenPixelSize.y, GUIUnitType::Auto });
			}
		}
		void GUIStack::elementUpdate(UpdateContext& _context)
		{
			if (checkBit(updateFlags, GUIElementUpdateFlag::TreeUpdateNeeded))
			{
				float pixelPenPosition = 0.0f;
				for (auto&& child : children)
				{
					if (!child->getVisible())
						continue;
					switch (orientation)
					{
						case StackOrientation::Vertical:
							if (child->getPosition().y.type == GUIUnitType::Auto)
							{
								const float childSize = child->unitToPixels(resolveUnitType(child->getSize().y, false), lastViewSize);
								child->setY(GUIUnit(pixelPenPosition, GUIUnitType::Auto));
								pixelPenPosition += childSize;
								pixelPenPosition += unitToPixels(getPadding(), _context.viewSize);
							}
							break;
						case StackOrientation::Horizontal:
							if (child->getPosition().x.type == GUIUnitType::Auto)
							{
								const float childSize = child->unitToPixels(resolveUnitType(child->getSize().x, true), lastViewSize);
								child->setX(GUIUnit(pixelPenPosition, GUIUnitType::Auto));
								pixelPenPosition += childSize;
								pixelPenPosition += unitToPixels(getPadding(), _context.viewSize);
							}
							break;

						default:
							se_assert(false);
							break;
					}
				}
			}
		}
	}
}
