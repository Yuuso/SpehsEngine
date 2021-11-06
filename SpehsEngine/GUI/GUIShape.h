#pragma once

#include "SpehsEngine/Core/HexColor.h"
#include "SpehsEngine/Graphics/Shape.h"
#include "SpehsEngine/GUI/GUIElement.h"


namespace se
{
	namespace gui
	{
		class GUIShape : public GUIElement
		{
		public:

													GUIShape();
			virtual									~GUIShape() = default;
													GUIShape(const GUIShape& _other) = delete;
			GUIShape&								operator=(const GUIShape& _other) = delete;
													GUIShape(GUIShape&& _other) = delete;
			GUIShape&								operator=(GUIShape&& _other) = delete;


			virtual bool							hitTest(const glm::vec2& _viewPoint) override;


			graphics::ShapeType						getShapeType() const;
			const Color&							getColor() const;
			std::shared_ptr<graphics::Material>		getMaterial() const;
			bool									getVisible() const;


			void									setShapeType(graphics::ShapeType _type);
			void									setColor(const Color& _color);
			void									setMaterial(std::shared_ptr<graphics::Material> _material);
			void									setVisible(bool _visible);

		protected:

			virtual void							elementUpdate(UpdateContext& _context) override;
			virtual void							onAddedParent() override;
			virtual void							onRemovedParent() override;
			virtual void							onRemovedFromView() override;
			virtual void							addToView() override;

		private:

			graphics::Shape shape;
		};
	}
}
