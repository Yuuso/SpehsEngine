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

													GUIElement() = default;
			virtual									~GUIElement() = default;
													GUIElement(const GUIElement& _other) = delete;
			GUIElement&								operator=(const GUIElement& _other) = delete;
													GUIElement(GUIElement&& _other) = delete;
			GUIElement&								operator=(GUIElement&& _other) = delete;


			virtual void							addChild(GUIElement& _element);
			virtual void							removeChild(GUIElement& _element);

			virtual bool							hitTest(const glm::vec2& _viewPoint);


			const GUIVec2&							getPosition() const;
			ZIndex									getZIndex() const;
			float									getRotation() const;
			const GUIVec2&							getSize() const;
			const glm::vec2&						getScale() const;
			bool									getClipping() const;
			const Margin&							getMargin() const;
			const Padding&							getPadding() const;
			const GUIVec2&							getAnchor() const;
			const GUIVec2&							getAlignment() const;
			bool									getVisible() const;


			void									setPosition(const GUIVec2& _position);
			void									setZIndex(ZIndex _zindex);
			void									setRotation(float _radians);
			void									setSize(const GUIVec2& _size);
			void									setScale(const glm::vec2& _scale);
			void									setClipping(bool _value);
			void									setMargin(const Margin& _margin);
			void									setPadding(const Padding& _padding);
			void									setVisible(bool _value);

			void									setAnchor(const GUIVec2& _anchor);
			void									setAnchor(VerticalAlignment _vertical, HorizontalAlignment _horizontal);
			void									setVerticalAnchor(VerticalAlignment _value);
			void									setHorizontalAnchor(HorizontalAlignment _value);

			void									setAlignment(const GUIVec2& _alignment);
			void									setAlignment(VerticalAlignment _vertical, HorizontalAlignment _horizontal);
			void									setVerticalAlignment(VerticalAlignment _value);
			void									setHorizontalAlignment(HorizontalAlignment _value);

		protected:

			friend class GUIView;

			virtual void							elementUpdate(UpdateContext&) {}
			virtual void							onAddedParent();
			virtual void							onRemovedParent();
			virtual void							onRemovedFromView() {}
			virtual void							addToView() {}
			virtual glm::vec2						getTransformOffset() { return {}; }

			GUIElementUpdateFlagsType				updateFlags		= 0;
			glm::mat4								globalTrasform	= glm::identity<glm::mat4>();
			graphics::Scissor						globalScissor;
			bool									globalVisible	= true;

		private:

			virtual void							update(UpdateContext& _context) final;
			virtual void							removeFromView() final;

			glm::vec2								getAnchorPositionOffset();
			glm::vec2								getAlignmentPositionOffset();
			glm::vec2								getMarginPaddingPositionOffset();

			GUIElement*								parent			= nullptr;
			std::vector<GUIElement*>				children;

			GUIVec2									position		= GUIVec2({ 0.0f, 0.0f }, GUIUnitType::Pixels);
			ZIndex									zindex			= 0;
			float									rotation		= 0.0f;
			GUIVec2									size			= GUIVec2({ 0.0f, 0.0f }, GUIUnitType::Pixels);
			glm::vec2								scale			= glm::vec2{ 1.0f, 1.0f };
			bool									clipping		= false;
			Margin									margin			= Margin(0.0f);
			Padding									padding			= Padding(0.0f);
			GUIVec2									anchor			= GUIVec2({ 0.0f, 0.0f }, GUIUnitType::Self);
			GUIVec2									alignment		= GUIVec2({ 0.0f, 0.0f }, GUIUnitType::Self);
			bool									visible			= true;
		};
	}
}
