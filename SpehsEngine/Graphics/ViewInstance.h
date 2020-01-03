#pragma once

#include "SpehsEngine/Graphics/View.h"
#include "SpehsEngine/Graphics/Camera.h"


namespace se
{
	namespace graphics
	{
		class ViewInstance
		{
		public:

			ViewInstance(const View& _view);
			~ViewInstance();

			ViewInstance(const ViewInstance& _other) = delete;
			ViewInstance& operator=(const ViewInstance& _other) = delete;

			ViewInstance(ViewInstance&& _other) = delete;
			ViewInstance& operator=(ViewInstance&& _other) = delete;

			bool operator==(const View& _other);


			void render();

		private:

			const View& view;
		};
	}
}
