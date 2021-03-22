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
			const uint32_t getClearColor() const;
			const ViewClearFlagsType getClearFlags() const;

			void setSize(const ViewSize& _size);
			void setOffset(const ViewSize& _offset);
			void setMSAAEnabled(const bool _enabled);
			void setClearColor(const uint32_t _color);
			void setClearFlags(const ViewClearFlagsType _flags);

		private:

			friend class ViewInternal;

			Scene& scene;
			Camera& camera;

			ViewSize size = ViewSize(1.0f, 1.0f, ViewSizeType::Relative);
			ViewSize offset = ViewSize(0.0f, 0.0f, ViewSizeType::Pixels);

			bool enableMSAA = true; // TODO: Test if this works, move to window if not
			ViewClearFlagsType clearFlags = ViewClearFlag::ClearColor
										  | ViewClearFlag::ClearDepth;
			uint32_t clearColor = 0x000000ff;

			boost::signals2::signal<void(void)> destroyedSignal;
		};
	}
}
