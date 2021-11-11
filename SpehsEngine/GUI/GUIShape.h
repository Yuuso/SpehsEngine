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


			graphics::ShapeType						getShapeType() const;
			const Color&							getColor() const;
			std::shared_ptr<graphics::Material>		getMaterial() const;


			void									setShapeType(graphics::ShapeType _type);
			void									setColor(const Color& _color);
			void									setMaterial(std::shared_ptr<graphics::Material> _material);

		protected:

			virtual void							elementUpdate(UpdateContext& _context) override;
			virtual void							onAddedParent() override;
			virtual void							onRemovedParent() override;
			virtual void							onRemovedFromView() override;
			virtual void							addToView() override;
			virtual glm::vec2						getTransformOffset() override;

		private:

			graphics::Shape shape;
		};
	}
}
