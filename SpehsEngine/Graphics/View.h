#pragma once

#include "SpehsEngine/Graphics/Scene.h"
#include "SpehsEngine/Graphics/Camera.h"
#include <memory>


namespace se
{
	namespace graphics
	{
		class View
		{
		public:

			View(const Scene& _scene, const Camera& _camera);
			~View();

			View(const View& _other) = delete;
			View& operator=(const View& _other) = delete;

			View(View&& _other) = delete;
			View& operator=(View&& _other) = delete;


			float getWidth() const;
			float getHeight() const;

		protected:

			friend class ViewInstance;

			const Scene& scene;
			const Camera& camera;

		private:

			float width;
			float height;
		};
	}
}
