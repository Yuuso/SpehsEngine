#pragma once


namespace se
{
	namespace graphics
	{
		class View;
		class FontManager;
		class ShaderManager;
	}

	namespace input
	{
		class InputManager;
	}

	namespace debug
	{
		// --------------
		// TODO: With ImGui or GUI
		// --------------
		class ScopeProfilerVisualizer
		{
		public:

			ScopeProfilerVisualizer(graphics::View& _view, graphics::FontManager& _fontManager, graphics::ShaderManager& _shaderManager, input::InputManager& _inputManager);
			~ScopeProfilerVisualizer();

			void update(const Time& deltaTime);

			void setRenderState(const bool visible);
			bool getRenderState() const;

			void setEnableUpdate(const bool enabled);
			bool getEnableUpdate() const;

			/* Set an override for preferred root section length. For example at 1/60 of a second when the root section is the frame. Used for section colouring purposes. */
			void setTargetRootSectionWidth(const Time* const time);
			void setTimeWindowWidth(const Time& time);
			void translateTimeWindowBegin(const Time& time);
			void setMaxThreadDataSectionCount(const size_t count);

		private:
			struct Impl;
			std::unique_ptr<Impl> impl;
		};
	}
}
