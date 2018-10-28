#pragma once
namespace se
{
	namespace rendering
	{
		class Window;
		class TextureManager;
		class BatchManager;
	}
	namespace input
	{
		class InputManager;
	}
	namespace time
	{
		class DeltaTimeSystem;
		class Time;
	}

	/*
	The GUI context is the caller's promise to the created GUIRectangle,
	that the provided context members will be valid throughout the GUIRectangle's lifetime.
	*/
	class GUIContext
	{
	public:
		GUIContext(se::rendering::BatchManager& _batchManager, input::InputManager& _inputManager, time::DeltaTimeSystem& _deltaTimeSystem);

		GUIContext& getGUIContext() { return *this; }
		const GUIContext& getGUIContext() const { return *this; }

		se::rendering::Window& getWindow();
		const se::rendering::Window& getWindow() const;
		se::rendering::BatchManager& getBatchManager();
		const se::rendering::BatchManager& getBatchManager() const;
		input::InputManager& getInputManager();
		const input::InputManager& getInputManager() const;
		time::DeltaTimeSystem& getDeltaTimeSystem();
		const time::DeltaTimeSystem& getDeltaTimeSystem() const;
		time::Time& getDeltaTime();
		const time::Time& getDeltaTime() const;
		float getDeltaSeconds() const;
		se::rendering::TextureManager& getTextureManager();
		const se::rendering::TextureManager& getTextureManager() const;

	private:
		se::rendering::BatchManager& batchManager;
		input::InputManager& inputManager;
		time::DeltaTimeSystem& deltaTimeSystem;
	};
}