#pragma once

#include "SpehsEngine/Graphics/View.h"
#include "SpehsEngine/Graphics/Camera.h"
#include "SpehsEngine/Graphics/Internal/InternalTypes.h"


namespace se
{
	namespace graphics
	{
		class ViewInternal
		{
		public:

			ViewInternal(View& _view);
			~ViewInternal();

			ViewInternal(const ViewInternal& _other) = delete;
			ViewInternal& operator=(const ViewInternal& _other) = delete;

			ViewInternal(ViewInternal&& _other) = delete;
			ViewInternal& operator=(ViewInternal&& _other) = delete;

			bool operator==(const View& _other) const;


			void render(RenderContext& _renderContext);
			void preRender(const bool _renderState, const bool _forceAllUpdates);
			void postRender(const bool _renderState);

			const bool wasDestroyed() const;

		private:

			void viewDestroyed();

			boost::signals2::scoped_connection viewDestroyedConnection;

			View* view;
			bool wasAdded = true;
		};
	}
}
