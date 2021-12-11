#pragma once

#include "SpehsEngine/Graphics/Primitive.h"
#include "SpehsEngine/GUI/Internal/InternalTypes.h"
#include "SpehsEngine/GUI/Types.h"


namespace se
{
	namespace gui
	{
		class GUIElement
		{
		public:

													GUIElement()						 = default;
			virtual									~GUIElement();
													GUIElement(const GUIElement& _other);
			GUIElement&								operator=(const GUIElement& _other)  = delete;
													GUIElement(GUIElement&& _other)		 = delete;
			GUIElement&								operator=(GUIElement&& _other)		 = delete;

			[[nodiscard]]
			virtual std::shared_ptr<GUIElement>		clone();


			size_t									addChild(std::shared_ptr<GUIElement> _element);
			void									removeChild(size_t _index);
			void									removeChild(GUIElement* _element);
			void									removeChild(std::shared_ptr<GUIElement> _element);
			void									clearChildren();
			std::shared_ptr<GUIElement>				getChild(size_t _index);
			size_t									getNumChildren() const;

			virtual bool							hitTest(const glm::vec2& _viewPoint);
			static GUIUnit							resolveUnitType(GUIUnit _unit, bool _isWidth);
			float									unitToPixels(GUIUnit _unit, const glm::vec2& _viewSize) const;
			glm::vec2								unitToPixels(const GUIVec2& _vec, const glm::vec2& _viewSize) const;


			const GUIVec2&							getPosition() const;
			ZIndex									getZIndex() const;
			ZIndex									getZValue() const;
			float									getRotation() const;
			const GUIVec2&							getSize() const;
			const glm::vec2&						getScale() const;
			bool									getClipping() const;
			const GUIVec2&							getAnchor() const;
			const GUIVec2&							getAlignment() const;
			bool									getVisible() const;
			GUIUnit									getPadding() const;

			void									setPosition(const GUIVec2& _position);
			void									setX(GUIUnit _x);
			void									setY(GUIUnit _y);
			void									setZIndex(ZIndex _zindex);
			void									setRotation(float _radians);
			void									setSize(const GUIVec2& _size);
			void									setWidth(GUIUnit _width);
			void									setHeight(GUIUnit _height);
			void									setScale(const glm::vec2& _scale);
			void									setClipping(bool _value);
			void									setVisible(bool _value);
			void									setAnchor(const GUIVec2& _anchor);
			void									setAnchor(VerticalAnchor _vertical, HorizontalAnchor _horizontal);
			void									setVerticalAnchor(VerticalAnchor _value);
			void									setHorizontalAnchor(HorizontalAnchor _value);
			void									setAlignment(const GUIVec2& _alignment);
			void									setAlignment(VerticalAlignment _vertical, HorizontalAlignment _horizontal);
			void									setVerticalAlignment(VerticalAlignment _value);
			void									setHorizontalAlignment(HorizontalAlignment _value);
			void									setPadding(GUIUnit _padding);

		protected:

			virtual void							elementPreUpdate()				{}
			virtual void							elementUpdate(UpdateContext&)	{}
			virtual void							onAddedParent()					{}
			virtual void							onRemovedParent()				{}
			virtual void							onAddedToView();
			virtual void							onRemovedFromView();
			virtual GUIVec2							getTransformOffset()			{ return GUIVec2({ 0.0f, 0.0f }, GUIUnitType::Pixels); }

			GUIElementUpdateFlagsType				updateFlags		= 0;
			glm::mat4								globalTrasform	= glm::identity<glm::mat4>();
			graphics::Scissor						globalScissor;
			bool									globalVisible	= true;
			glm::vec2								lastViewSize;

			GUIElement*								parent			= nullptr;
			std::vector<std::shared_ptr<GUIElement>> children;

		private:

			friend class GUIView;

			virtual void							preUpdate() final;
			virtual void							update(UpdateContext& _context) final;
			virtual void							removeFromView() final;
			virtual void							addToView() final;

			glm::vec2								getAnchorPositionOffset(const glm::vec2& _viewSize);
			glm::vec2								getAlignmentPositionOffset(const glm::vec2& _viewSize);

			GUIVec2									position		= GUIVec2({ 0.0f, 0.0f }, GUIUnitType::Auto);
			ZIndex									zindex			= 0;
			float									rotation		= 0.0f;
			GUIVec2									size			= GUIVec2({ 0.0f, 0.0f }, GUIUnitType::Auto);
			glm::vec2								scale			= glm::vec2{ 1.0f, 1.0f };
			bool									clipping		= false;
			GUIVec2									anchor			= GUIVec2({ 0.0f, 0.0f }, GUIUnitType::Self);
			GUIVec2									alignment		= GUIVec2({ 0.0f, 0.0f }, GUIUnitType::Parent);
			GUIUnit									padding			= GUIUnit(0.0f, GUIUnitType::Pixels);
			bool									visible			= true;
			bool									isRootElement	= false;
		};
	}
}
