#pragma once
namespace spehs
{
	class Window;
	class TextureManager;
	class BatchManager;
	class InputManager;
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
		GUIContext(BatchManager& _batchManager, InputManager& _inputManager, time::DeltaTimeSystem& _deltaTimeSystem);

		GUIContext& getGUIContext() { return *this; }
		const GUIContext& getGUIContext() const { return *this; }

		Window& getWindow();
		const Window& getWindow() const;
		BatchManager& getBatchManager();
		const BatchManager& getBatchManager() const;
		InputManager& getInputManager();
		const InputManager& getInputManager() const;
		time::DeltaTimeSystem& getDeltaTimeSystem();
		const time::DeltaTimeSystem& getDeltaTimeSystem() const;
		time::Time& getDeltaTime();
		const time::Time& getDeltaTime() const;
		float getDeltaSeconds() const;
		TextureManager& getTextureManager();
		const TextureManager& getTextureManager() const;

	private:
		BatchManager& batchManager;
		InputManager& inputManager;
		time::DeltaTimeSystem& deltaTimeSystem;
	};
}