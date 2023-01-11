#pragma once

#include "SpehsEngine/GUI/GUIElement.h"


namespace se
{
	namespace legacygui
	{
		class GUIStack : public GUIElement
		{
		public:

													GUIStack()							= default;
			virtual									~GUIStack()							= default;
													GUIStack(const GUIStack& _other);
													GUIStack(GUIStack&& _other)			= delete;
			GUIStack&								operator=(const GUIStack& _other)	= delete;
			GUIStack&								operator=(GUIStack&& _other)		= delete;

			[[nodiscard]]
			virtual std::shared_ptr<GUIElement>		clone() override;


			StackOrientation						getOrientation() const;

			void									setOrientation(StackOrientation _orientation);

		protected:

			virtual void							elementPreUpdate(UpdateContext& _context) override;
			virtual void							elementUpdate(UpdateContext& _context) override;

		private:

			StackOrientation orientation = StackOrientation::Vertical;
		};
	}
}
