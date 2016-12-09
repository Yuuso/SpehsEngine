#include <iostream>
#include "inputManager.h"
#include "Time.h"
#include "Geometry.h"
#include "applicationData.h"
#include "GUIWindow.h"
#include "GUITextField.h"
#include "GUIRectangleList.h"
#define EXIT_WIDTH 20
//#define DOCK_BORDER 50
#define DOUBLE_CLICK_TIME 200
/*Strech state*/
#define STRECH_STATE_HORIZONTAL 1
#define STRECH_STATE_VERTICAL 2
#define STRECH_STATE_N 4
#define STRECH_STATE_E 8
#define STRECH_STATE_S 16
#define STRECH_STATE_W 32
#define STRECH_CORNER_PERCENTAGE 0.1f

namespace spehs
{
	//Static variables (public for modification at user's will)
	int GUIWindow::upBorder = 0;
	int GUIWindow::downBorder = 0;
	int GUIWindow::leftBorder = 0;
	int GUIWindow::rightBorder = 0;
	int GUIWindow::strechWidth = 5;
	glm::vec4 GUIWindow::strechColorFocused = glm::vec4(1.0f, 0.22f, 0.0f, 1.0f);
	glm::vec4 GUIWindow::strechColorUnfocused = glm::vec4(0.05f, 0.05f, 0.7f, 1.0f);
	glm::vec4 GUIWindow::headerBackgroundColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	glm::vec4 GUIWindow::headerStringColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	glm::vec4 GUIWindow::exitBackgroundColor = glm::vec4(0.6f, 0.05f, 0.05f, 1.0f);
	glm::vec4 GUIWindow::exitStringColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

	GUIWindow::GUIWindow() : doubleClickTimer(0), strechState(0)
	{
		//Header bar
		header = new GUIRectangle(-1);
		header->setParent(this);
		header->setPositionLocal(0, size.y);
		header->setColor(headerBackgroundColor);
		header->setStringColor(headerStringColor);
		header->setJustification(GUIRECT_TEXT_JUSTIFICATION_LEFT);
		header->setStringSize(applicationData->GUITextSize + 2);
		header->setDepth(getDepth() + 1);
		header->setRenderState(checkState(GUIRECT_OPEN_BIT) && getRenderState());

		//Exit button
		exit = new GUIRectangle(-2);
		exit->setParent(this);
		exit->setPositionLocal(size.x - EXIT_WIDTH, size.y);
		exit->setSize(EXIT_WIDTH, header->getHeight());
		exit->setID(1);
		exit->setColor(exitBackgroundColor);
		exit->setString("X");
		exit->setStringColor(exitStringColor);
		exit->setJustification(GUIRECT_TEXT_JUSTIFICATION_CENTER);
		exit->setDepth(getDepth() + 1);
		exit->setRenderState(checkState(GUIRECT_OPEN_BIT) && getRenderState());

		//Strech background rectangle
		strech = new GUIRectangle();
		strech->setParent(this);
		strech->setPositionLocal(-strechWidth, -strechWidth);
		strech->disableState(GUIRECT_HOVER_COLOR);
		strech->setDepth(getDepth() - 1);
		strech->setRenderState(checkState(GUIRECT_OPEN_BIT) && getRenderState());

		setSize(minSize);
		disableState(GUIRECT_HOVER_COLOR);
		limitWithinMainWindow();
	}
	GUIWindow::~GUIWindow()
	{
		delete header;
		delete exit;
		delete strech;
	}
	void GUIWindow::inputUpdate()
	{
		//Run update in all members
		GUIRectangleContainer::inputUpdate();
		header->inputUpdate();
		exit->inputUpdate();
		strech->inputUpdate();
		if (strech->getMouseHover())
		{
			enableBit(state, GUIRECT_MOUSE_HOVER);
			enableBit(state, GUIRECT_MOUSE_HOVER_CONTAINER);
		}

		////DRAGGING
		//Handle previous frame dragging
		if (checkState(GUIRECT_DRAGGING_BIT) && inputManager->isKeyDown(MOUSE_BUTTON_LEFT))
			translate(inputManager->getMouseMovementX(), inputManager->getMouseMovementY());
		//Set dragging boolean
		if (checkState(GUIRECT_ENABLED_BIT) && !checkState(GUIRECT_DRAGGING_BIT) && header->getMouseHover() && inputManager->isKeyPressed(MOUSE_BUTTON_LEFT))
			enableState(GUIRECT_DRAGGING_BIT);//Begin dragging
		else if (checkState(GUIRECT_DRAGGING_BIT) && !inputManager->isKeyDown(MOUSE_BUTTON_LEFT))
		{//Stop dragging
#ifdef DOCK_BORDER
			//Check docking
			if (inputManager->getMouseX() < DOCK_BORDER)
			{//Dock left
				setSize(minSize.x, applicationData->getWindowHeight() - upBorder - downBorder - header->getHeight() - 2 * strechWidth);
				setPositionLocal(strechWidth + leftBorder, strechWidth + downBorder);
			}
			else if (inputManager->getMouseX() > applicationData->getWindowWidth() - DOCK_BORDER)
			{//Dock right
				setSize(minSize.x, applicationData->getWindowHeight() - upBorder - downBorder - header->getWidth() - 2 * strechWidth);
				setPositionLocal(applicationData->getWindowWidth() - size.x - strechWidth - rightBorder, strechWidth + downBorder);
			}

			if (inputManager->getMouseY() < DOCK_BORDER)
			{//Dock down
				setSize(applicationData->getWindowWidth() - 2 * strechWidth - leftBorder - rightBorder, minSize.y);
				setPositionLocal(strechWidth + leftBorder, strechWidth + downBorder);
			}
			else if (inputManager->getMouseY() > applicationData->getWindowHeight() - DOCK_BORDER)
			{//Dock up
				setSize(applicationData->getWindowWidth() - 2 * strechWidth - leftBorder - rightBorder, minSize.y);
				setPositionLocal(strechWidth + leftBorder, applicationData->getWindowHeight() - header->getHeight() - strechWidth - size.y - upBorder);
			}
#endif
			limitWithinMainWindow();
			disableState(GUIRECT_DRAGGING_BIT);
		}

		////STRECHING
		//Handle previous frame strech
		if (/*Starting streching requires window to be enabled for interaction! -> No need for checking herer*/checkState(GUIRECT_STRECHING_BIT) && inputManager->isKeyDown(MOUSE_BUTTON_LEFT))
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
					if (inputManager->getMouseX() > getXGlobal() + size.x / 2.0f)
						breakFromStrech = true;
				}
				else
				{//Strech began from the right side of the window
					if (inputManager->getMouseX() < getXGlobal() + size.x / 2.0f)
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
					if (inputManager->getMouseY() < getYGlobal() + size.y / 2.0f + header->getHeight() / 2.0f)
						breakFromStrech = true;
				}
				else
				{//Strech began from the lower side of the window
					if (inputManager->getMouseY() > getYGlobal() + size.y / 2.0f + header->getHeight() / 2.0f)
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
		if (checkState(GUIRECT_ENABLED_BIT) &&
			!checkState(GUIRECT_STRECHING_BIT) &&
			inputManager->isKeyPressed(MOUSE_BUTTON_LEFT) &&
			mouseOverStrechArea())
		{//Begin streching

			////Define state variables
			enableState(GUIRECT_STRECHING_BIT);
			strechState = 0;
			//Horizontal
			if (inputManager->getMouseX() < getXGlobal() + size.x * STRECH_CORNER_PERCENTAGE)
			{
				//Enable horizontal streching
				enableBit(strechState, STRECH_STATE_HORIZONTAL);
				//Take record that the streching began from the western side of the window
				enableBit(strechState, STRECH_STATE_W);
			}
			else if (inputManager->getMouseX() > getXGlobal() + size.x * (1.0 - STRECH_CORNER_PERCENTAGE))
			{
				//Enable horizontal streching
				enableBit(strechState, STRECH_STATE_HORIZONTAL);
				//Take record that the streching began from the eastern side of the window
				enableBit(strechState, STRECH_STATE_E);
			}
			//Vertical
			if (inputManager->getMouseY() < getYGlobal() + size.y * STRECH_CORNER_PERCENTAGE)
			{
				//Enable vertical streching
				enableBit(strechState, STRECH_STATE_VERTICAL);
				//Take record that the streching began from the southern side of the window
				enableBit(strechState, STRECH_STATE_S);
			}
			else if (inputManager->getMouseY() > getYGlobal() + size.y * (1.0 - STRECH_CORNER_PERCENTAGE))
			{
				//Enable vertical streching
				enableBit(strechState, STRECH_STATE_VERTICAL);
				//Take record that the streching began from the northern side of the window
				enableBit(strechState, STRECH_STATE_N);
			}
		}
		else if (checkState(GUIRECT_STRECHING_BIT) &&
			(!checkState(GUIRECT_ENABLED_BIT) || !inputManager->isKeyDown(MOUSE_BUTTON_LEFT)))
		{//Conditions not met to continue streching, set to false
			disableState(GUIRECT_STRECHING_BIT);
			strechState = 0;
		}

		////Header double clicking
		//Timer decrease
		if (doubleClickTimer > 0)
			doubleClickTimer -= time::getDeltaTimeAsMilliseconds();
		//Check header double click
		if (checkState(GUIRECT_ENABLED_BIT) && inputManager->isKeyPressed(MOUSE_BUTTON_LEFT) && header->getMouseHover())
		{//Header has been clicked
			if (doubleClickTimer > 0)
			{//Header double click detected
				disableState(GUIRECT_DRAGGING_BIT);

				if (size == minSize)
				{//Set to full window

					//Set size according to application data's window dimensions
					setSize(applicationData->getWindowWidth() - rightBorder - leftBorder, applicationData->getWindowHeight() - upBorder - downBorder - header->getHeight());
					setPositionLocal(0, 0);

					//Position window so that it won't go out of the application window
					limitWithinMainWindow();
				}
				else
				{//Set to min size

					//Take record of the window's old center position so that resized window can be nicely centered at the same position
					glm::vec2 oldCenterPos(getXGlobal() + size.x / 2.0f, getYGlobal() + size.y / 2.0f);

					//Rezise to min size
					setSize(minSize);

					//Reposition relative to old center position (recorded earlier)
					setPositionLocal(oldCenterPos.x - size.x / 2.0f, oldCenterPos.y - size.y / 2.0f);

					//Position window so that it won't go out of the application window
					limitWithinMainWindow();
				}
			}
			else
				doubleClickTimer = DOUBLE_CLICK_TIME;
		}

		//Check exit button
		if (checkState(GUIRECT_ENABLED_BIT) && inputManager->isKeyPressed(MOUSE_BUTTON_LEFT) && exit->getMouseHover())
			close();

	}
	void GUIWindow::visualUpdate()
	{
		//Check size and positioning before drawing strech
		if (!checkBit(state, GUIRECT_MIN_SIZE_UPDATED_BIT))
			updateMinSize();
		if (!checkBit(state, GUIRECT_SCALE_UPDATED_BIT))
			updateScale();
		if (!checkBit(state, GUIRECT_POSITION_UPDATED_BIT))
			updatePosition();

		strech->visualUpdate();
		GUIRectangleContainer::visualUpdate();
		header->visualUpdate();
		exit->visualUpdate();
	}
	void GUIWindow::onEnable()
	{
		enableStateRecursive(GUIRECT_ENABLED_BIT);
		strech->setColor(strechColorFocused);
		GUIRectangleContainer::onEnable();
	}
	void GUIWindow::onDisable()
	{
		disableState(GUIRECT_STRECHING_BIT);
		disableState(GUIRECT_DRAGGING_BIT);
		strech->setColor(strechColorUnfocused);
		GUIRectangleContainer::onDisable();
	}
	bool GUIWindow::close()
	{
		if (!checkState(GUIRECT_OPEN_BIT))
			return false;
		disableBit(state, GUIRECT_OPEN_BIT);
		disableBit(state, GUIRECT_MOUSE_HOVER);
		disableBit(state, GUIRECT_MOUSE_HOVER_CONTAINER);
		disable();
		setRenderState(false);
		return true;
	}
	bool GUIWindow::open()
	{
		if (checkState(GUIRECT_OPEN_BIT))
			return false;
		enableBit(state, GUIRECT_OPEN_BIT);
		setRenderState(true);
		refresh();
		std::cout << "\nWindow opened: " << header->getString();
		return true;
	}
	void GUIWindow::updateMinSize()
	{
		//Initialize min dimensions
		header->updateMinSize();
		minSize.x = header->getMinWidth() + exit->getWidth();
		minSize.y = header->getMinHeight();

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
	void GUIWindow::updateScale()
	{
		GUIRectangle::updateScale();

		//Background strech rectangle
		strech->setSize(size.x + 2 * strechWidth, size.y + 2 * strechWidth);

		//Resize and reposition header
		header->setWidth(size.x - exit->getWidth());

		//Resize exit
		exit->setHeight(header->getHeight());

		////Resize and position elements
		if (elements.size() == 0)
			return;
		bool minY = false;//Use minimal y value for every element
		if (size.y == minSize.y)
		{//Automatically use minimal size for every element
			for (unsigned i = 0; i < elements.size(); i++)
				elements[i]->setSize(size.x, elements[i]->getMinHeight());
		}
		else
		{//Use scaled size for every element
			int hAllocated(0), hElement;
			float scalePercentage = (size.y - header->getHeight()) / float(minSize.y - header->getMinHeight());//How many % of min size should each element scale
			for (std::vector<spehs::GUIRectangle*>::iterator i = elements.begin(); i != elements.end() - 1; i++)
			{
				hElement = scalePercentage * (*i)->getMinHeight();
				hAllocated += hElement;
				(*i)->setSize(size.x, hElement);
			}
			//Allocate excess h to last element
			elements.back()->setSize(size.x, size.y - header->getHeight() - hAllocated);
		}

		disableBit(state, GUIRECT_POSITION_UPDATED_BIT);
	}
	void GUIWindow::updatePosition()
	{
		GUIRectangle::updatePosition();

		//Reposition and update exit button
		exit->setPositionLocal(size.x - exit->getWidth(), size.y - header->getHeight());

		//Reposition and update header
		header->setPositionLocal(0, size.y - header->getHeight());

		//Update strech position
		strech->disableState(GUIRECT_POSITION_UPDATED_BIT);

		////Reposition and update all elements
		if (elements.size() == 0)
			return;
		//Position first element
		elements[0]->setPositionLocal(0, size.y - header->getHeight() - elements[0]->getHeight());
		for (unsigned i = 1; i < elements.size(); i++)
		{
			elements[i]->setPositionLocal(0, elements[i - 1]->getYLocal() - elements[i]->getHeight());
		}
	}
	void GUIWindow::limitWithinMainWindow()
	{//Prevent window going out of application window

		//Limit size
		if (size.y + header->getHeight() > applicationData->getWindowHeight() - upBorder - downBorder)
		{//Window too high
			setSize(size.x, applicationData->getWindowHeight() - header->getHeight() - upBorder - downBorder);
		}
		if (size.x > applicationData->getWindowWidth() - leftBorder - rightBorder)
		{//Window too wide
			setSize(applicationData->getWindowWidth() - leftBorder - rightBorder, size.y);
		}

		//////Limit position
		////Y
		if (getYGlobal() > applicationData->getWindowHeight() - size.y - upBorder - header->getHeight())
		{//Window too up
			setPositionGlobal(position.x, applicationData->getWindowHeight() - size.y - upBorder - header->getHeight());
		}
		else if (getYGlobal() < downBorder)
		{//Window too down
			setPositionGlobal(position.x, downBorder);
		}
		////X
		if (getXGlobal() > applicationData->getWindowWidth() - size.x - rightBorder)
		{//Window too right
			setPositionGlobal(applicationData->getWindowWidth() - size.x - rightBorder, position.y);
		}
		else if (getXGlobal() < leftBorder)
		{//Window too left
			setPositionGlobal(leftBorder, position.y);
		}

		updateScale();
		updatePosition();
	}
	void GUIWindow::setDepth(int16_t depth)
	{
		GUIRectangleContainer::setDepth(depth);
		strech->setDepth(depth - 1);
		header->setDepth(depth + 1);
		exit->setDepth(depth + 1);
	}
	void GUIWindow::setRenderState(const bool _state)
	{
		GUIRectangleContainer::setRenderState(_state);
		strech->setRenderState(_state);
		header->setRenderState(_state);
		exit->setRenderState(_state);
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
		if (inputManager->getMouseX() < getXGlobal())
			return true;
		if (inputManager->getMouseY() < getYGlobal())
			return true;
		if (inputManager->getMouseX() > getXGlobal() + size.x)
			return true;
		if (inputManager->getMouseY() > getYGlobal() + size.y)
			return true;
		return false;
	}
	void GUIWindow::refresh()
	{
		disableBit(state, GUIRECT_MIN_SIZE_UPDATED_BIT);
		disableBit(state, GUIRECT_SCALE_UPDATED_BIT);
		disableBit(state, GUIRECT_POSITION_UPDATED_BIT);
		disableBit(state, GUIRECT_REFRESH_BIT);
	}
}