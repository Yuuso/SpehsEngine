#pragma once

#include "SpehsEngine/Graphics/View.h"
#include "SpehsEngine/Graphics/Camera.h"
#include "SpehsEngine/Graphics/Internal/InternalTypes.h"


namespace se
{
	namespace graphics
	{
		class ViewInstance
		{
		public:

			ViewInstance(View& _view);
			~ViewInstance();

			ViewInstance(const ViewInstance& _other) = delete;
			ViewInstance& operator=(const ViewInstance& _other) = delete;

			ViewInstance(ViewInstance&& _other) = delete;
			ViewInstance& operator=(ViewInstance&& _other) = delete;

			bool operator==(const View& _other) const;


			void render(RenderContext& _renderContext);
			void preRender();
			void postRender();

			const bool wasDestroyed() const;

		private:

			void viewDestroyed();

			boost::signals2::scoped_connection viewDestroyedConnection;

			View* view;
		};
	}
}
