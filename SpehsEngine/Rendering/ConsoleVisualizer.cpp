#include "stdafx.h"
#include <algorithm>
#include <iostream>
#include <atomic>
#include <mutex>

#include <boost/bind.hpp>

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/Serializable.h"
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Input/Input.h"
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/Rendering/ConsoleVisualizer.h"
#include "SpehsEngine/Rendering/Camera2D.h"
#include "SpehsEngine/Rendering/FontManager.h"
#include "SpehsEngine/Rendering/Text.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/Window.h"
#include "SpehsEngine/Rendering/Polygon.h"

//Misc
#define CONSOLE_COMMANDS_KEPT_IN_MEMORY 10
#define LOG_LINES_KEPT_IN_MEMORY 25
#define CONSOLE_BORDER 5
#define BACKSPACE_INITIAL_INTERVAL 0.5f
#define BACKSPACE_INTERVAL 75
#define FADE_OUT_TIME 2.5f
//FPS
#define FPS_REFRESH_RATE 5

namespace se
{
	namespace rendering
	{
		ConsoleVisualizer::ConsoleVisualizer(Console& _console, se::InputManager& _inputManager, BatchManager& _batchManager)
			: console(_console)
			, inputManager(_inputManager)
			, batchManager(_batchManager)
			, keyboardRecorder(_inputManager)
			, backgroundShade(*_batchManager.createPolygon(Shape::BUTTON, 0, 1.0f, 1.0f))
			, statsText(*_batchManager.createText())
			, carotText(*_batchManager.createText())
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);

			//Background shade
			backgroundShade.setCameraMatrixState(false);
			backgroundShade.setPosition(0.0f, 0.0f);
			backgroundShade.setColor(se::Color());
			backgroundShade.setRenderState(true);
			backgroundShade.setAlpha(204);

			//Stats text
			statsText.setColor(Console::defaultLogColor);

			//Carot text
			carotText.setColor(Console::defaultLogColor);
			carotText.setString("|");

			//Update some values
			setFont("Fonts/Anonymous.ttf", 12);
			setPlaneDepth(0);
			setLineCapacity(10);
			scrollState = int(getMaxScrollState());
			updateLines = true;

			//Connect log connection
			logConnection = console.logSignal.connect(std::bind(&ConsoleVisualizer::logCallback, this, std::placeholders::_1));
		}

		ConsoleVisualizer::~ConsoleVisualizer()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			backgroundShade.destroy();
			statsText.destroy();
			carotText.destroy();
			for (size_t i = 0; i < lines.size(); i++)
				lines[i]->destroy();
			lines.clear();
		}

		void ConsoleVisualizer::open()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (openState)
				return;
			openState = true;
			updateLines = true;
			lines.back()->setString("");
		}

		void ConsoleVisualizer::close()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (!openState)
				return;
			openState = false;
			updateLines = true;
		}

		void ConsoleVisualizer::setRenderState(const bool _state)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			backgroundShade.setRenderState(_state);
		}

		bool ConsoleVisualizer::getRenderState() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return backgroundShade.getRenderState();
		}

		void ConsoleVisualizer::setPlaneDepth(const int16_t depth)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			backgroundShade.setPlaneDepth(depth);
			statsText.setPlaneDepth(depth + 1);
			carotText.setPlaneDepth(depth + 1);
			for (size_t i = 0; i < lines.size(); i++)
				lines[i]->setPlaneDepth(depth + 1);
		}

		int16_t ConsoleVisualizer::getPlaneDepth() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return backgroundShade.getPlaneDepth();
		}

		void ConsoleVisualizer::setShowStats(const bool show)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			showStats = show;
		}

		bool ConsoleVisualizer::getShowStats() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return showStats;
		}

		bool ConsoleVisualizer::isOpen()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return openState;
		}

		void ConsoleVisualizer::update(const time::Time& deltaTime)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);

			if (openState)
			{
				//Carot visibility timer
				carotTimer += deltaTime;

				keyboardRecorder.update(deltaTime);
				if (keyboardRecorder.characterInput.size() > 0 || keyboardRecorder.commandInput.size() > 0)
					inputTime = time::now();

				//Append character input
				std::string inputString = lines.back()->getString();
				for (size_t i = 0; i < keyboardRecorder.characterInput.size(); i++)
					inputString += keyboardRecorder.characterInput[i];

				//Command input
				for (size_t i = 0; i < keyboardRecorder.commandInput.size(); i++)
				{
					switch (keyboardRecorder.commandInput[i])
					{
					case KEYBOARD_RETURN:
					{
						const std::string input = lines.back()->getString();
						if (input.size() > 0)
							console.execute(input);
						close();
					}
					break;
					case KEYBOARD_BACKSPACE:
						if (!inputString.empty())
							inputString.pop_back();
						break;
					case KEYBOARD_ESCAPE:
						close();
						break;
					case KEYBOARD_UP:
						//TODO!
						break;
					case KEYBOARD_DOWN:
						//TODO!
						break;
					case KEYBOARD_PAGEUP:
						if (scrollState > 0)
						{
							scrollState--;
							updateLines = true;
						}
						break;
					case KEYBOARD_PAGEDOWN:
						if (scrollState < (int)getMaxScrollState())
						{
							scrollState++;
							updateLines = true;
						}
						break;
					case KEYBOARD_HOME:
						scrollState = 0;
						updateLines = true;
						break;
					case KEYBOARD_END:
						const int maxScrollState = (int)getMaxScrollState();
						if (scrollState != maxScrollState)
						{
							scrollState = maxScrollState;
							updateLines = true;
						}
						break;
					}
				}

				if (inputString != lines.back()->getString())
				{
					lines.back()->setString(inputString);
					updateCarotPosition();
				}
			}
			else
			{
				//Console is not open, check opening key combination
				if (inputManager.isKeyDown(KEYBOARD_LCTRL) && inputManager.isKeyDown(KEYBOARD_LALT) && inputManager.isKeyDown(KEYBOARD_BACKSPACE))
				{
					open();
				}
			}
		}

		void ConsoleVisualizer::render(const std::string& customDebugText)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			const bool renderState = getRenderState();
			statsText.setRenderState(renderState && showStats);
			static const time::Time carotInterval = time::fromSeconds(1.0f);
			carotText.setRenderState(renderState && openState && ((((carotTimer.value / carotInterval.value) % 2) == 0) || (time::now() - inputTime) < se::time::fromSeconds(1.0f)));
			for (size_t i = 0; i < lines.size(); i++)
				lines[i]->setRenderState(renderState);

			if (renderState)
			{
				//Lines
				if (updateLines)
				{
					se_assert(lines.size() > 0);

					//Update positions
					const size_t visibleLineCount = std::min(lines.size(), console.getLineCount() + (openState ? 1u : 0u));
					float y = (float)(lines.back()->getFontHeight() * (int)lines.size());
					for (size_t i = 0; i < lines.size(); i++)
					{
						y -= (float)lines[i]->getFontHeight();
						lines[i]->setPosition(glm::vec2(CONSOLE_BORDER, CONSOLE_BORDER + (float)y));
					}

					//Update strings
					const size_t count = openState ? (lines.size() - 1) : lines.size();
					for (size_t i = 0; i < count; i++)
					{
						const size_t consoleLineIndex = std::max(0, scrollState + (int)i + (openState ? 1 : 0));
						if (consoleLineIndex < console.getLineCount())
						{
							const Console::Line& line = console.getLine(consoleLineIndex);
							lines[i]->setString(line.string);
							lines[i]->setColor(line.color);
						}
						else
						{
							lines[i]->setString("");
						}
					}

					//Carot position needs to be updated
					updateCarotPosition();

					updateLines = false;
				}

				//Background shade size
				if (console.getLineCount() == 0 || !openState)
				{
					backgroundShade.resize(0.0f, 0.0f);
				}
				else
				{
					int w = 0;
					int h = 0;
					for (size_t i = 0; i < lines.size(); i++)
					{
						if (lines[i]->getString().size() > 0)
						{
							w = std::max(w, (int)lines[i]->getTextWidth());
							h += (int)lines[i]->getTextHeight();
						}
					}
					w += CONSOLE_BORDER * 2;
					h += CONSOLE_BORDER * 2;
					if (abs(backgroundShade.getWidth() - w) > 0.9f || abs(backgroundShade.getHeight() - h) > 0.9f)
						backgroundShade.resize((float)w, (float)h);
				}

				//Determine render lap time
				renderLapTimes.push_back(renderLapTimer.get());
				if (renderLapTimes.size() > 20)
					renderLapTimes.erase(renderLapTimes.begin());
				const time::Time oldestLapTime = renderLapTimes.front();
				time::Time renderLapTimesRelativeSum = 0;
				for (size_t i = 0; i < renderLapTimes.size(); i++)
					renderLapTimesRelativeSum += renderLapTimes[i] - oldestLapTime;
				const time::Time averageLapTime = oldestLapTime + time::Time(time::TimeValueType(float(renderLapTimesRelativeSum) / float(renderLapTimes.size())));

				//Status text
				statsText.setString("FPS: " + std::to_string(int(std::round(1.0f / averageLapTime.asSeconds()))) + "\n" + customDebugText);
				statsText.setPosition(glm::vec2(CONSOLE_BORDER, (float)batchManager.window.getHeight() - statsText.getTextHeight() - CONSOLE_BORDER));
			}
		}

		void ConsoleVisualizer::setFont(const std::string& fontPath, const int fontSize)
		{
			setFont(batchManager.fontManager.getFont(fontPath, fontSize));
		}

		void ConsoleVisualizer::setFont(Font* fontParam)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (font == fontParam)
				return;
			font = fontParam;
			statsText.setFont(font);
			carotText.setFont(font);
			for (size_t i = 0; i < lines.size(); i++)
				lines[i]->setFont(font);
		}

		void ConsoleVisualizer::setLineCapacity(const size_t capacity)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (capacity == lines.size())
				return;

			if (capacity > lines.size())
			{
				const size_t count = capacity - lines.size();
				lines.reserve(capacity);
				for (size_t i = 0; i < count; i++)
				{
					lines.push_back(batchManager.createText());
					lines.back()->setFont(font);
					lines.back()->setPlaneDepth(backgroundShade.getPlaneDepth() + 1);
					lines.back()->setAlpha(229);
				}
			}
			else if (lines.size() > capacity)
			{
				const size_t count = lines.size() - capacity;
				const size_t backIndex = lines.size() - 1;
				for (size_t i = 0; i < count; i++)
					lines[backIndex - i]->destroy();
				lines.resize(capacity);
			}

			//Update scroll state
			scrollState = std::min(scrollState, (int)getMaxScrollState());

			//Update caret position
			updateCarotPosition();

			//Queue line update
			updateLines = true;
		}

		size_t ConsoleVisualizer::getLineCapacity() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return lines.size();
		}

		void ConsoleVisualizer::logCallback(const Console::Line& line)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			updateLines = true;
			const size_t maxScrollState = getMaxScrollState();
			if (maxScrollState != 0 && scrollState == maxScrollState - 1)
				scrollState++;
		}

		size_t ConsoleVisualizer::getInputLineIndex() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			se_assert(lines.size() > 0);
			return lines.size() - 1;
		}

		size_t ConsoleVisualizer::getMaxScrollState() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return std::max(0, int(console.getLineCount() - lines.size()));
		}

		void ConsoleVisualizer::updateCarotPosition()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			carotText.setPosition(lines.back()->getPosition() + glm::vec2(std::max(0.0f, lines.back()->getTextWidth() - carotText.getTextWidth() / 2), 0.0f));
		}
	}
}