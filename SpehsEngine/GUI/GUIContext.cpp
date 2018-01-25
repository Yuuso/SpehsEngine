#include "SpehsEngine/GUI/GUIContext.h"
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Rendering/BatchManager.h"

namespace spehs
{
	GUIContext::GUIContext(BatchManager& _batchManager, InputManager& _inputManager, time::DeltaTimeSystem& _deltaTimeSystem)
		: batchManager(_batchManager)
		, inputManager(_inputManager)
		, deltaTimeSystem(_deltaTimeSystem)
	{

	}

	Window& GUIContext::getWindow()
	{
		return batchManager.window;
	}

	const Window& GUIContext::getWindow() const
	{
		return batchManager.window;
	}

	BatchManager& GUIContext::getBatchManager()
	{
		return batchManager;
	}

	const BatchManager& GUIContext::getBatchManager() const
	{
		return batchManager;
	}

	InputManager& GUIContext::getInputManager()
	{
		return inputManager;
	}

	const InputManager& GUIContext::getInputManager() const
	{
		return inputManager;
	}

	time::DeltaTimeSystem& GUIContext::getDeltaTimeSystem()
	{
		return deltaTimeSystem;
	}

	const time::DeltaTimeSystem& GUIContext::getDeltaTimeSystem() const
	{
		return deltaTimeSystem;
	}

	time::Time& GUIContext::getDeltaTime()
	{
		return deltaTimeSystem.deltaTime;
	}

	const time::Time& GUIContext::getDeltaTime() const
	{
		return deltaTimeSystem.deltaTime;
	}

	float GUIContext::getDeltaSeconds() const
	{
		return deltaTimeSystem.deltaSeconds;
	}

	TextureManager& GUIContext::getTextureManager()
	{
		return batchManager.textureManager;
	}

	const TextureManager& GUIContext::getTextureManager() const
	{
		return batchManager.textureManager;
	}
}