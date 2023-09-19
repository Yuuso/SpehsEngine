#pragma once

#include "SpehsEngine/Core/HexColor.h"
#include "SpehsEngine/Graphics/Shape.h"
#include "SpehsEngine/GUI/GUIElement.h"


namespace se
{
	namespace legacygui
	{
		class GUIShape : public GUIElement
		{
		public:

													GUIShape();
													GUIShape(graphics::ShapeType _shapeType);
			virtual									~GUIShape()							= default;
													GUIShape(const GUIShape& _other);
													GUIShape(GUIShape&& _other)			= delete;
			GUIShape&								operator=(const GUIShape& _other)	= delete;
			GUIShape&								operator=(GUIShape&& _other)		= delete;

			[[nodiscard]]
			virtual std::shared_ptr<GUIElement>		clone() override;


			const Color&							getColor() const;
			const std::string_view					getTexture() const;

			void									setColor(const Color& _color);
			void									setAlpha(float _alpha);
			void									setTexture(std::string_view _name);
			void									setLayerMask(bool _value);

		protected:

			virtual void							elementUpdate(UpdateContext& _context) override;
			virtual void							onAddedParent() override;
			virtual void							onRemovedParent() override;
			virtual void							onRemovedFromView() override;
			virtual void							onAddedToView() override;
			virtual GUIVec2							getTransformOffset() override;

		private:

			void									initShape(UpdateContext& _context, graphics::ShapeType _shapeType);

			graphics::Shape shape;
			std::optional<graphics::ShapeType> initShapeType;
		};
	}
}
