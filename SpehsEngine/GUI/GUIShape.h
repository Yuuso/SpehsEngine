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
			void									setTexture(std::string_view _name);

		protected:

			virtual void							elementUpdate(UpdateContext& _context) override;
			virtual void							onAddedParent() override;
			virtual void							onRemovedParent() override;
			virtual void							onRemovedFromView() override;
			virtual void							onAddedToView() override;
			virtual GUIVec2							getTransformOffset() override;

		private:

			void									initShape();

			graphics::Shape shape;
		};
	}
}
