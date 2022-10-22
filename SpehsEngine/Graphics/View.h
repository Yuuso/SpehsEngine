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


			Scene& getScene();
			Camera& getCamera();
			const ViewSize& getSize() const;
			const ViewSize& getOffset() const;
			bool getMSAAEnabled() const;
			uint32_t getClearColor() const;
			ViewClearFlagsType getClearFlags() const;
			DrawCallSortOrder getDrawCallSortOrder() const;

			void setSize(const ViewSize& _size);
			void setOffset(const ViewSize& _offset);
			void setMSAAEnabled(bool _enabled);
			void setClearColor(uint32_t _hexColor);
			void setClearFlags(ViewClearFlagsType _flags);
			void setDrawCallSortOrder(DrawCallSortOrder _order);

			boost::signals2::scoped_connection connectToPreRenderSignal(std::function<void(glm::vec2)> _func);

		private:

			friend class ViewInternal;

			Scene& scene;
			Camera& camera;

			ViewSize size = ViewSize(1.0f, 1.0f, ViewSizeType::Relative);
			ViewSize offset = ViewSize(0.0f, 0.0f, ViewSizeType::Pixels);

			bool enableMSAA = true; // TODO: Test if this works, move to window if not
			ViewClearFlagsType clearFlags = ViewClearFlag::ClearColor | ViewClearFlag::ClearDepth | ViewClearFlag::ClearStencil;
			uint32_t clearColor = 0x000000ff;
			DrawCallSortOrder drawCallSortOrder = DrawCallSortOrder::Default;

			boost::signals2::signal<void(glm::vec2)> onPreRenderSignal; // param: view size in pixels
			boost::signals2::signal<void(void)> destroyedSignal;
		};
	}
}
