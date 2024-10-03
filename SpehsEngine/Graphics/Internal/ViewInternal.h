#pragma once


namespace se
{
	namespace graphics
	{
		class View;
		class WindowInternal;

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
			void preRender(const WindowInternal* _currentWindow, const bool _renderState, const bool _forceAllUpdates);
			void postRender(const bool _renderState);

			const bool wasDestroyed() const;

		private:

			void viewDestroyed();
			void calculateRenderSize(const WindowInternal* _currentWindow);

			ScopedConnection viewDestroyedConnection;

			View* view;
			bool wasAdded = true;
			glm::vec2 renderSize;
			glm::vec2 renderOffset;
		};
	}
}
