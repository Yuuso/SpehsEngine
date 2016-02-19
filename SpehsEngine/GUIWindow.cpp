#include <iostream>
#include "inputManager.h"
#include "PolygonBatch.h"
#include "Time.h"
#include "Geometry.h"
#include "applicationData.h"
#include "GUIWindow.h"
#include "GUITextField.h"
#include "GUIRectangleList.h"
#define HEADER_HEIGHT 20
#define STRECH_BORDER 5
#define DOCK_BORDER 50
#define DOUBLE_CLICK_TIME 200
//Borders next to the application window where GUI windows should not stand
#define UP_BORDER 10
#define DOWN_BORDER 10
#define RIGHT_BORDER 10
#define LEFT_BORDER 10
/*Strech state*/
#define STRECH_STATE_HORIZONTAL 1
#define STRECH_STATE_VERTICAL 2
#define STRECH_STATE_N 4
#define STRECH_STATE_E 8
#define STRECH_STATE_S 16
#define STRECH_STATE_W 32
#define STRECH_CORNER_PERCENTAGE 0.1f

namespace SpehsEngine
{
	GUIWindow::GUIWindow() : doubleClickTimer(0), strechState(0)
	{
		//Set size
		setSize(minSize);

		//Header bar
		header = new GUIRectangle(-1);
		header->setParent(this);
		header->setPosition(0, size.y);
		header->setColor(255, 255, 255);
		header->setJustification(GUIRECT_TEXT_JUSTIFICATION_LEFT);
		header->setStringSize(11);

		//Exit button
		exit = new GUIRectangle(-2);
		exit->setParent(this);
		exit->setPosition(size.x - HEADER_HEIGHT, size.y);
		exit->setSize(HEADER_HEIGHT, HEADER_HEIGHT);
		exit->setID(1);
		exit->setColor(255, 0, 0);
		exit->setStringSize(11);
		exit->setString("X");

		//Strech background rectangle
		strech = new GUIRectangle();
		strech->setParent(this);
		strech->setPosition(-STRECH_BORDER, -STRECH_BORDER);
		strech->disableState(GUIRECT_HOVER_COLOR);

		//State
		disableBit(state, GUIRECT_HOVER_COLOR);
		disableBit(state, GUIRECT_OPEN);
		//By default, lose focus
		loseFocus();
	}
	GUIWindow::~GUIWindow()
	{
		delete header;
		delete exit;
		delete strech;
	}
	void GUIWindow::update()
	{

		//Clicking within window -> gaining focus
		if (!isFocused() && isOpen() && inputManager->isKeyPressed(MOUSE_BUTTON_LEFT) && (strech->updateMouseHover() || getMouseHover()))
			gainFocus();

		//Run update in all members
		GUIRectangleContainer::update();
		header->update();
		exit->update();
		strech->update();
		if (strech->getMouseHover())
		{
			enableBit(state, GUIRECT_MOUSE_HOVER);
			enableBit(state, GUIRECT_MOUSE_HOVER_CONTAINER);
		}

		////DRAGGING
		//Handle previous frame dragging
		if (isDragging() && inputManager->isKeyDown(MOUSE_BUTTON_LEFT))
			translate(inputManager->getMouseMovementX(), inputManager->getMouseMovementY());
		//Set dragging boolean
		if (!checkBit(state, GUIRECT_DRAGGING) && checkBit(state, GUIRECT_FOCUSED) && header->getMouseHover() && inputManager->isKeyPressed(MOUSE_BUTTON_LEFT))
			enableBit(state, GUIRECT_DRAGGING);//Begin dragging
		else if (isDragging() && isFocused() && inputManager->isKeyDown(MOUSE_BUTTON_LEFT))
			enableBit(state, GUIRECT_DRAGGING);//Continue dragging
		else if (isDragging())
		{//Stop dragging

			//Check docking
			if (inputManager->getMouseX() < DOCK_BORDER)
			{//Dock left
				setSize(minSize.x, applicationData->getWindowHeight() - UP_BORDER - DOWN_BORDER - HEADER_HEIGHT - 2 * STRECH_BORDER);
				setPosition(STRECH_BORDER + LEFT_BORDER, STRECH_BORDER + DOWN_BORDER);
			}
			else if (inputManager->getMouseX() > applicationData->getWindowWidth() - DOCK_BORDER)
			{//Dock right
				setSize(minSize.x, applicationData->getWindowHeight() - UP_BORDER - DOWN_BORDER - HEADER_HEIGHT - 2 * STRECH_BORDER);
				setPosition(applicationData->getWindowWidth() - size.x - STRECH_BORDER - RIGHT_BORDER, STRECH_BORDER + DOWN_BORDER);
			}

			if (inputManager->getMouseY() < DOCK_BORDER)
			{//Dock down
				setSize(applicationData->getWindowWidth() - 2 * STRECH_BORDER - LEFT_BORDER - RIGHT_BORDER, minSize.y);
				setPosition(STRECH_BORDER + LEFT_BORDER, STRECH_BORDER + DOWN_BORDER);
			}
			else if (inputManager->getMouseY() > applicationData->getWindowHeight() - DOCK_BORDER)
			{//Dock up
				setSize(applicationData->getWindowWidth() - 2 * STRECH_BORDER - LEFT_BORDER - RIGHT_BORDER, minSize.y);
				setPosition(STRECH_BORDER + LEFT_BORDER, applicationData->getWindowHeight() - header->getHeight() - STRECH_BORDER - size.y - UP_BORDER);
			}

			limitWithinMainWindow();
			disableBit(state, GUIRECT_DRAGGING);
		}

		////STRECHING
		//Handle previous frame strech
		if (isStreching() && inputManager->isKeyDown(MOUSE_BUTTON_LEFT))
		{
			//Take record of current dimensions
			float w = size.x;
			float h = size.y;
			bool breakFromStrech = false;

			//Check horizontal strech movement
			if (checkBit(strechState, STRECH_STATE_HORIZONTAL))
			{
				//Check if mouse has crossed over to the opposite side of the window -> break from strech
				if (checkBit(strechState, STRECH_STATE_W))
				{//Strech began from left side of the window
					if (inputManager->getMouseX() > getX() + size.x / 2.0f)
						breakFromStrech = true;
				}
				else
				{//Strech began from the right side of the window
					if (inputManager->getMouseX() < getX() + size.x / 2.0f)
						breakFromStrech = true;
				}

				//Strech window based on mouse movement
				if (!breakFromStrech)
				{
					if (inputManager->getMouseX() > position.x + size.x / 2.0f)
					{
						w += inputManager->getMouseMovementX();
					}
					else
					{
						w -= inputManager->getMouseMovementX();
						if (abs(size.x - minSize.x) > 0.01f)
							translate(inputManager->getMouseMovementX(), 0);
					}
				}
			}

			//Check vertical strech movement
			if (!breakFromStrech && checkBit(strechState, STRECH_STATE_VERTICAL))
			{
				//Check if mouse has crossed over to the opposite side of the window -> break from strech
				if (checkBit(strechState, STRECH_STATE_N))
				{//Strech began from upper side of the window
					if (inputManager->getMouseY() < getY() + size.y / 2.0f + header->getHeight() / 2.0f)
						breakFromStrech = true;
				}
				else
				{//Strech began from the lower side of the window
					if (inputManager->getMouseY() > getY() + size.y / 2.0f + header->getHeight() / 2.0f)
						breakFromStrech = true;
				}

				//Strech window based on mouse movement
				if (!breakFromStrech)
				{
					if (inputManager->getMouseY() > position.y + size.y / 2.0f)
					{
						h += inputManager->getMouseMovementY();
					}
					else
					{
						h -= inputManager->getMouseMovement().y;
						if (abs(size.y - minSize.y) > 0.01f)
						translate(0, inputManager->getMouseMovementY());
					}
				}
			}

			//Set size to the calculated dimensions
			setSize(w, h);

			//position within main application window
			limitWithinMainWindow();
		}
		//Handle strech state bit
		if (!checkBit(state, GUIRECT_STRECHING) &&
			checkBit(state, GUIRECT_FOCUSED) &&
			inputManager->isKeyPressed(MOUSE_BUTTON_LEFT) &&
			mouseOverStrechArea())
		{//Begin streching

			////Define state variables
			enableBit(state, GUIRECT_STRECHING);
			strechState = 0;
			//Horizontal
			if (inputManager->getMouseX() < getX() + size.x * STRECH_CORNER_PERCENTAGE)
			{
				//Enable horizontal streching
				enableBit(strechState, STRECH_STATE_HORIZONTAL);
				//Take record that the streching began from the western side of the window
				enableBit(strechState, STRECH_STATE_W);
			}
			else if (inputManager->getMouseX() > getX() + size.x * (1.0 - STRECH_CORNER_PERCENTAGE))
			{
				//Enable horizontal streching
				enableBit(strechState, STRECH_STATE_HORIZONTAL);
				//Take record that the streching began from the eastern side of the window
				enableBit(strechState, STRECH_STATE_E);
			}
			//Vertical
			if (inputManager->getMouseY() < getY() + size.y * STRECH_CORNER_PERCENTAGE)
			{
				//Enable vertical streching
				enableBit(strechState, STRECH_STATE_VERTICAL);
				//Take record that the streching began from the southern side of the window
				enableBit(strechState, STRECH_STATE_S);
			}
			else if (inputManager->getMouseY() > getY() + size.y * (1.0 - STRECH_CORNER_PERCENTAGE))
			{
				//Enable vertical streching
				enableBit(strechState, STRECH_STATE_VERTICAL);
				//Take record that the streching began from the northern side of the window
				enableBit(strechState, STRECH_STATE_N);
			}
		}
		else if (isStreching() &&
			(!isFocused() ||
			!inputManager->isKeyDown(MOUSE_BUTTON_LEFT)))
		{//Conditions not met to continue streching, set to false
			disableBit(state, GUIRECT_STRECHING);
			strechState = 0;
		}

		////Header double clicking
		//Timer decrease
		if (doubleClickTimer > 0)
			doubleClickTimer -= SpehsEngine::deltaTime;
		//Check header double click
		if (isFocused() && inputManager->isKeyPressed(MOUSE_BUTTON_LEFT) && header->getMouseHover())
		{//Header has been clicked
			if (doubleClickTimer > 0)
			{//Header double click detected
				disableBit(state, GUIRECT_DRAGGING);

				if (size == minSize)
				{//Set to full window

					//Set size according to application data's window dimensions
					setSize(applicationData->getWindowWidth() - RIGHT_BORDER - LEFT_BORDER, applicationData->getWindowHeight() - UP_BORDER - DOWN_BORDER - HEADER_HEIGHT);
					setPosition(0, 0);

					//Position window so that it won't go out of the application window
					limitWithinMainWindow();
				}
				else
				{//Set to min size

					//Take record of the window's old center position so that resized window can be nicely centered at the same position
					glm::vec2 oldCenterPos(getX() + size.x / 2.0f, getY() + size.y / 2.0f);

					//Rezise to min size
					setSize(minSize);

					//Reposition relative to old center position (recorded earlier)
					setPosition(oldCenterPos.x - size.x / 2.0f, oldCenterPos.y - size.y / 2.0f);

					//Position window so that it won't go out of the application window
					limitWithinMainWindow();
				}
			}
			else
				doubleClickTimer = DOUBLE_CLICK_TIME;
		}

		//Check exit button
		if (inputManager->isKeyPressed(MOUSE_BUTTON_LEFT) && exit->getMouseHover())
			close();

	}
	void GUIWindow::render()
	{
		if (!isOpen())
			return;

		//Check size and positioning before drawing strech
		if (!checkBit(state, GUIRECT_SCALED))
			updateScale();
		if (!checkBit(state, GUIRECT_POSITIONED))
			updatePosition();

		strech->render();
		GUIRectangleContainer::render();
		header->render();
		exit->render();
	}
	void GUIWindow::postRender()
	{
		strech->postRender();
		header->postRender();
		exit->postRender();
		GUIRectangleContainer::postRender();
	}
	void GUIWindow::gainFocus()
	{//Function called whenever this window receives focus
		if (isFocused())
			return;

		enableStateRecursive(GUIRECT_FOCUSED);
		strech->setColor(255, 110, 0);
		std::cout << "\nWindow gained focus: " << header->getString();
	}
	void GUIWindow::loseFocus()
	{//Function called whenever this window loses focus

		//Return if there was no focus to begin with
		if (!isFocused())
			return;

		disableBit(state, GUIRECT_STRECHING);
		disableBit(state, GUIRECT_DRAGGING);
		strech->setColor(0, 0, 0);
		std::cout << "\nWindow lost focus: " << header->getString();

		GUIRectangleContainer::loseFocus();
	}
	void GUIWindow::close()
	{
		disableBit(state, GUIRECT_OPEN);
		loseFocus();
		std::cout << "\nWindow closed: " << header->getString();
	}
	void GUIWindow::open()
	{
		enableBit(state, GUIRECT_OPEN);
		refresh();
		std::cout << "\nWindow opened: " << header->getString();
	}
	void GUIWindow::updatePosition()
	{
		GUIRectangle::updatePosition();

		//Reposition and update exit button
		exit->setPosition(size.x - HEADER_HEIGHT, size.y);
		exit->updatePosition();

		//Reposition and update header
		header->setPosition(0, size.y);
		header->updatePosition();

		//Update strech position
		strech->updatePosition();

		//Reposition and update all elements
		for (int i = int(elements.size()) - 1; i >= 0; i--)
		{
			if (i == elements.size() - 1)
				elements[i]->setPosition(0, 0);
			else
				elements[i]->setPosition(0, elements[i + 1]->getLocalY() + elements[i + 1]->getHeight());
		}
		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->updatePosition();
	}
	void GUIWindow::updateScale()
	{
		GUIRectangle::updateScale();

		//Background strech rectangle
		strech->setSize(size.x + 2 * STRECH_BORDER, size.y + 2 * STRECH_BORDER + HEADER_HEIGHT);
		
		//Resize and reposition header
		header->setSize(size.x - HEADER_HEIGHT, HEADER_HEIGHT);

		////Resize and position elements
		bool minY = false;//Use minimal y value for every element
		if (size.y == minSize.y)
			minY = true;
		//Resize
		for (unsigned i = 0; i < elements.size(); i++)
		{
			if (minY)
				elements[i]->setSize(size.x, elements[i]->getMinHeight());
			else
				elements[i]->setSize(size.x, elements[i]->getMinHeight() / float(minSize.y) * size.y);
		}

		disableBit(state, GUIRECT_POSITIONED);
	}
	void GUIWindow::limitWithinMainWindow()
	{//Prevent window going out of application window

		//Limit size
		if (size.y + HEADER_HEIGHT > applicationData->getWindowHeight() - UP_BORDER - DOWN_BORDER)
		{//Window too high
			if (size.x > applicationData->getWindowWidth() - LEFT_BORDER - RIGHT_BORDER)
				setSize(applicationData->getWindowWidth() - LEFT_BORDER - RIGHT_BORDER, applicationData->getWindowHeight() - HEADER_HEIGHT - UP_BORDER - DOWN_BORDER);
			else
				setSize(size.x, applicationData->getWindowHeight() - HEADER_HEIGHT - UP_BORDER - DOWN_BORDER);
		}
		else if (size.x > applicationData->getWindowWidth() - LEFT_BORDER - RIGHT_BORDER)
		{//Window too wide
			setSize(applicationData->getWindowWidth() - LEFT_BORDER - RIGHT_BORDER, size.y);
		}

		//////Limit position
		////Y
		if (getY() > applicationData->getWindowHeight() - size.y - UP_BORDER - HEADER_HEIGHT)
		{//Window too up
			setPosition(position.x, applicationData->getWindowHeight() - size.y - UP_BORDER - HEADER_HEIGHT);
		}
		else if (getY() < DOWN_BORDER)
		{//Window too down
			setPosition(position.x, DOWN_BORDER);
		}
		////X
		if (getX() > applicationData->getWindowWidth() - size.x - RIGHT_BORDER)
		{//Window too right
			setPosition(applicationData->getWindowWidth() - size.x - RIGHT_BORDER, position.y);
		}
		else if (getX() < LEFT_BORDER)
		{//Window too left
			setPosition(LEFT_BORDER, position.y);
		}
	}
	void GUIWindow::updateMinSize()
	{
		//Initialize min dimensions
		header->updateMinSize();
		minSize.x = header->getMinWidth() + exit->getWidth();
		minSize.y = 0;

		//Go through every element...
		for (unsigned i = 0; i < elements.size(); i++)
		{
			elements[i]->updateMinSize();

			//If element requires more width, change min width into element's requirement
			if (elements[i]->getMinWidth() > minSize.x)
				minSize.x = elements[i]->getMinWidth();
			//Increase min height by element's minimal required height
			minSize.y += elements[i]->getMinHeight();
		}
	}
	void GUIWindow::disableStateRecursive(GUIRECT_STATE_TYPE stateBit)
	{
		strech->disableState(stateBit);
		header->disableState(stateBit);
		exit->disableState(stateBit);
		GUIRectangleContainer::disableStateRecursive(stateBit);
	}
	void GUIWindow::enableStateRecursive(GUIRECT_STATE_TYPE stateBit)
	{
		strech->enableState(stateBit);
		header->enableState(stateBit);
		exit->enableState(stateBit);
		GUIRectangleContainer::enableStateRecursive(stateBit);
	}
	void GUIWindow::disableState(GUIRECT_STATE_TYPE stateBit)
	{
		strech->disableState(stateBit);
		header->disableState(stateBit);
		exit->disableState(stateBit);
		GUIRectangleContainer::disableState(stateBit);
	}
	void GUIWindow::enableState(GUIRECT_STATE_TYPE stateBit)
	{
		strech->enableState(stateBit);
		header->enableState(stateBit);
		exit->enableState(stateBit);
		GUIRectangleContainer::enableState(stateBit);
	}
	bool GUIWindow::mouseOverStrechArea()
	{
		//Mouse not overlapping strech rect, no hovering
		if (!strech->getMouseHover())
			return false;

		//Check each border individually
		if (inputManager->getMouseX() < getX())
			return true;
		if (inputManager->getMouseY() < getY())
			return true;
		if (inputManager->getMouseX() > getX() + size.x)
			return true;
		if (inputManager->getMouseY() > getY() + size.y + header->getHeight())
			return true;
		return false;
	}
	void GUIWindow::refresh()
	{
		disableBit(state, GUIRECT_SCALED);
		disableBit(state, GUIRECT_POSITIONED);
		disableBit(state, GUIRECT_MIN_SIZE_UPDATED);
	}
}