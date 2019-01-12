#include "stdafx.h"
#include "SpehsEngine/GUI/GUIContext.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/DeltaTimeSystem.h"
#include "SpehsEngine/Rendering/BatchManager.h"

namespace se
{
	GUIContext::GUIContext(se::rendering::BatchManager& _batchManager, input::InputManager& _inputManager, time::DeltaTimeSystem& _deltaTimeSystem)
		: batchManager(_batchManager)
		, inputManager(_inputManager)
		, deltaTimeSystem(_deltaTimeSystem)
	{

	}

	se::rendering::Window& GUIContext::getWindow()
	{
		return batchManager.window;
	}

	const se::rendering::Window& GUIContext::getWindow() const
	{
		return batchManager.window;
	}

	se::rendering::BatchManager& GUIContext::getBatchManager()
	{
		return batchManager;
	}

	const se::rendering::BatchManager& GUIContext::getBatchManager() const
	{
		return batchManager;
	}

	input::InputManager& GUIContext::getInputManager()
	{
		return inputManager;
	}

	const input::InputManager& GUIContext::getInputManager() const
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

	se::rendering::TextureManager& GUIContext::getTextureManager()
	{
		return batchManager.textureManager;
	}

	const se::rendering::TextureManager& GUIContext::getTextureManager() const
	{
		return batchManager.textureManager;
	}
}