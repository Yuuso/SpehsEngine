#pragma once

#include "boost/signals2.hpp"
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

			View(Scene& _scene, Camera& _camera);
			~View();

			View(const View& _other) = delete;
			View& operator=(const View& _other) = delete;

			View(View&& _other) = delete;
			View& operator=(View&& _other) = delete;


			const ViewSize& getSize() const;
			const ViewSize& getOffset() const;
			const bool getMSAAEnabled() const;

			void setSize(const ViewSize& _size);
			void setOffset(const ViewSize& _offset);
			void setMSAAEnabled(const bool _enabled);

		private:

			friend class ViewInstance;

			Scene& scene;
			Camera& camera;

			ViewSize size = ViewSize(1.0f, 1.0f, ViewSize::ViewSizeType::Relative);
			ViewSize offset = ViewSize(0.0f, 0.0f, ViewSize::ViewSizeType::Pixels);

			bool enableMSAA = true; // TODO: Test if this works, move to window if not

			boost::signals2::signal<void(void)> destroyedSignal;
		};
	}
}
