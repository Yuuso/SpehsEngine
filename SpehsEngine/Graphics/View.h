#pragma once

#include "SpehsEngine/Graphics/Scene.h"
#include "SpehsEngine/Graphics/Camera.h"
#include "SpehsEngine/Graphics/Types.h"
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


			const ViewSize& getSize() const;
			const ViewSize& getOffset() const;

			void setSize(const ViewSize& _size);
			void setOffset(const ViewSize& _offset);

		protected:

			friend class ViewInstance;

			const Scene& scene;
			const Camera& camera;

		private:

			ViewSize size = ViewSize(1.0f, 1.0f, ViewSize::ViewSizeType::Relative);
			ViewSize offset = ViewSize(0.0f, 0.0f, ViewSize::ViewSizeType::Pixels);
		};
	}
}
