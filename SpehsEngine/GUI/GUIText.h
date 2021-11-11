#pragma once

#include "SpehsEngine/GUI/GUIElement.h"
#include "SpehsEngine/Graphics/Text.h"


namespace se
{
	namespace gui
	{
		class GUIText : public GUIElement
		{
		public:

													GUIText();
			virtual									~GUIText() = default;
													GUIText(const GUIText& _other) = delete;
			GUIText&								operator=(const GUIText& _other) = delete;
													GUIText(GUIText&& _other) = delete;
			GUIText&								operator=(GUIText&& _other) = delete;


			const Color&							getColor() const;
			std::shared_ptr<graphics::Material>		getMaterial() const;

			const size_t							length() const;
			std::string								getPlainText() const;
			const size_t							getPenPosition() const;
			const float								getLineSpacing() const;


			void									setColor(const Color& _color);
			void									setMaterial(std::shared_ptr<graphics::Material> _material);

			void									setLineSpacing(const float _amount);
			void									movePen(const int _movement);

			void									insert(const std::string& _text); // utf-8
			void									clear();

		protected:

			virtual void							elementUpdate(UpdateContext& _context) override;
			virtual void							onAddedParent() override;
			virtual void							onRemovedParent() override;
			virtual void							onRemovedFromView() override;
			virtual void							addToView() override;
			virtual glm::vec2						getTransformOffset() override;

		private:

			void									updateDimensions();

			graphics::Text text;
		};
	}
}
