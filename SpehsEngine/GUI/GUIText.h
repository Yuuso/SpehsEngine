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
			virtual									~GUIText()							= default;
													GUIText(const GUIText& _other);
			GUIText&								operator=(const GUIText& _other)	= delete;
													GUIText(GUIText&& _other)			= delete;
			GUIText&								operator=(GUIText&& _other)			= delete;

			[[nodiscard]]
			virtual std::shared_ptr<GUIElement>		clone() override;


			const Color&							getColor() const;
			std::string_view						getFont() const;
			std::string								getPlainText() const;
			const size_t							getPenPosition() const;
			const float								getLineSpacing() const;
			const size_t							getTextLength() const;

			void									setColor(const Color& _color);
			void									setFont(std::string_view _font);
			void									setLineSpacing(const float _amount);
			void									movePen(const int _movement);
			void									insert(const std::string& _text); // utf-8
			void									clear();

		protected:

			virtual void							elementPreUpdate(UpdateContext& _context) override;
			virtual void							elementUpdate(UpdateContext& _context) override;
			virtual void							onAddedParent() override;
			virtual void							onRemovedParent() override;
			virtual void							onRemovedFromView() override;
			virtual void							onAddedToView() override;
			virtual GUIVec2							getTransformOffset() override;

		private:

			void									initText();
			bool									updateDimensions();
			glm::vec3								getRenderScale(const glm::vec2& _viewSize);

			gfx::Text text;
			bool needUpdateDimensions = false;
		};
	}
}
