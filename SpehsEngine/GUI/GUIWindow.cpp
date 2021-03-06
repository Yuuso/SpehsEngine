#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/DeltaTimeSystem.h"
#include "SpehsEngine/Math/Geometry.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/Window.h"
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/Input/InputLib.h"
#include "SpehsEngine/GUI/GUIWindow.h"
#include "SpehsEngine/GUI/GUIStringEditor.h"
#include "SpehsEngine/GUI/GUIRectangleScrollList.h"
#define EXIT_WIDTH 20
//#define DOCK_BORDER 50
#define DOUBLE_CLICK_TIME 200
/*Strech state*/
#define STRECH_STATE_HORIZONTAL int8_t(1)
#define STRECH_STATE_VERTICAL int8_t(2)
#define STRECH_STATE_N int8_t(4)
#define STRECH_STATE_E int8_t(8)
#define STRECH_STATE_S int8_t(16)
#define STRECH_STATE_W int8_t(32)
#define STRECH_CORNER_PERCENTAGE 0.1f

namespace se
{
	//Static variables (public for modification at user's will)
	int GUIWindow::upBorder = 0;
	int GUIWindow::downBorder = 0;
	int GUIWindow::leftBorder = 0;
	int GUIWindow::rightBorder = 0;
	int GUIWindow::strechWidth = 5;
	se::Color GUIWindow::strechColorFocused(255, 57, 0, 255);
	se::Color GUIWindow::strechColorUnfocused(13, 13, 179, 255);
	se::Color GUIWindow::headerBackgroundColor(204, 204, 204, 255);
	se::Color GUIWindow::headerStringColor(51, 51, 51, 255);
	se::Color GUIWindow::exitBackgroundColor(153, 13, 13, 255);
	se::Color GUIWindow::exitStringColor(51, 51, 51, 255);

	GUIWindow::GUIWindow(GUIContext& context)
		: GUIRectangleContainer(context)
		, doubleClickTimer(0)
		, strechState(0)
		, refreshRequests(0)
	{
		//Header bar
		header = new GUIRectangle(context);
		header->setID(-1);
		header->setParent(this);
		header->setPositionLocal(0, size.y);
		header->setColor(headerBackgroundColor);
		header->setStringColor(headerStringColor);
		header->setJustification(GUIRECT_TEXT_JUSTIFICATION_LEFT_BIT);
		header->setStringSize(14);
		header->setDepth(getDepth() + 1);
		header->setRenderState(checkState(GUIRECT_OPEN_BIT) && getRenderState());
		addElement(header);

		//Exit button
		exit = new GUIRectangle(context);
		exit->setID(-2);
		exit->setParent(this);
		exit->setPositionLocal(size.x - EXIT_WIDTH, size.y);
		exit->setSize(EXIT_WIDTH, header->getHeight());
		exit->setID(1);
		exit->setColor(exitBackgroundColor);
		exit->setString("X");
		exit->setStringColor(exitStringColor);
		exit->setJustification(GUIRECT_TEXT_JUSTIFICATION_CENTER_BIT);
		exit->setDepth(getDepth() + 1);
		exit->setRenderState(checkState(GUIRECT_OPEN_BIT) && getRenderState());
		addElement(exit);

		//Strech background rectangle
		strech = new GUIRectangle(context);
		strech->setParent(this);
		strech->setPositionLocal(-strechWidth, -strechWidth);
		strech->disableState(GUIRECT_HOVER_COLOR_BIT);
		strech->setDepth(getDepth() - 1);
		strech->setRenderState(checkState(GUIRECT_OPEN_BIT) && getRenderState());
		addElement(strech);

		setSize(minSize);
		disableState(GUIRECT_HOVER_COLOR_BIT);
		enableState(GUIWINDOW_LIMIT_WITHIN_MAIN_WINDOW_BIT);
	}

	GUIWindow::~GUIWindow()
	{
	}

	void GUIWindow::clear()
	{
		//Do not clear header, strech and exit elements located on front of the elements vector
		for (unsigned i = GUIWINDOW_BASE_ELEMENT_COUNT; i < elements.size(); i++)
			delete elements[i];
		elements.resize(GUIWINDOW_BASE_ELEMENT_COUNT);
	}

	void GUIWindow::inputUpdate()
	{
		GUIRectangleContainer::inputUpdate();

		////DRAGGING
		//Handle previous frame dragging
		if (checkState(GUIWINDOW_DRAGGING_BIT) && getInputManager().isKeyDown(MOUSE_BUTTON_LEFT))
			translate(int(getInputManager().getMouseMovementX()), int(getInputManager().getMouseMovementY()));
		//Set dragging boolean
		if (getInputEnabled() && !checkState(GUIWINDOW_DRAGGING_BIT) && header->getMouseHover() && getInputManager().isKeyPressed(MOUSE_BUTTON_LEFT))
			enableState(GUIWINDOW_DRAGGING_BIT);//Begin dragging
		else if (checkState(GUIWINDOW_DRAGGING_BIT) && !getInputManager().isKeyDown(MOUSE_BUTTON_LEFT))
		{//Stop dragging
#ifdef DOCK_BORDER
			//Check docking
			if (getInputManager().getMouseX() < DOCK_BORDER)
			{//Dock left
				setSize(minSize.x, batchManager.window.getHeight() - upBorder - downBorder - header->getHeight() - 2 * strechWidth);
				setPositionLocal(strechWidth + leftBorder, strechWidth + downBorder);
			}
			else if (getInputManager().getMouseX() > batchManager.window.getWidth() - DOCK_BORDER)
			{//Dock right
				setSize(minSize.x, batchManager.window.getHeight() - upBorder - downBorder - header->getWidth() - 2 * strechWidth);
				setPositionLocal(batchManager.window.getWidth() - size.x - strechWidth - rightBorder, strechWidth + downBorder);
			}

			if (getInputManager().getMouseY() < DOCK_BORDER)
			{//Dock down
				setSize(batchManager.window.getWidth() - 2 * strechWidth - leftBorder - rightBorder, minSize.y);
				setPositionLocal(strechWidth + leftBorder, strechWidth + downBorder);
			}
			else if (getInputManager().getMouseY() > batchManager.window.getHeight() - DOCK_BORDER)
			{//Dock up
				setSize(batchManager.window.getWidth() - 2 * strechWidth - leftBorder - rightBorder, minSize.y);
				setPositionLocal(strechWidth + leftBorder, batchManager.window.getHeight() - header->getHeight() - strechWidth - size.y - upBorder);
			}
#endif
			enableState(GUIWINDOW_LIMIT_WITHIN_MAIN_WINDOW_BIT);
			disableState(GUIWINDOW_DRAGGING_BIT);
		}

		////STRECHING
		//Handle previous frame strech
		if (/*Starting streching requires window to be enabled for interaction! -> No need for checking here*/checkState(GUIWINDOW_STRECHING_BIT) && getInputManager().isKeyDown(MOUSE_BUTTON_LEFT))
		{
			//Take record of current dimensions
			float w = float(size.x);
			float h = float(size.y);
			bool breakFromStrech = false;

			//Check horizontal strech movement
			if (checkBit(strechState, STRECH_STATE_HORIZONTAL))
			{
				//Check if mouse has crossed over to the opposite side of the window -> break from strech
				if (checkBit(strechState, STRECH_STATE_W))
				{//Strech began from left side of the window
					if (getInputManager().getMouseX() > getXGlobal() + size.x / 2.0f)
						breakFromStrech = true;
				}
				else
				{//Strech began from the right side of the window
					if (getInputManager().getMouseX() < getXGlobal() + size.x / 2.0f)
						breakFromStrech = true;
				}

				//Strech window based on mouse movement
				if (!breakFromStrech)
				{
					if (getInputManager().getMouseX() > position.x + size.x / 2.0f)
					{
						w += getInputManager().getMouseMovementX();
					}
					else
					{
						w -= getInputManager().getMouseMovementX();
						if (abs(size.x - minSize.x) > 0.01f)
							translate(int(getInputManager().getMouseMovementX()), 0);
					}
				}
			}

			//Check vertical strech movement
			if (!breakFromStrech && checkBit(strechState, STRECH_STATE_VERTICAL))
			{
				//Check if mouse has crossed over to the opposite side of the window -> break from strech
				if (checkBit(strechState, STRECH_STATE_N))
				{//Strech began from upper side of the window
					if (getInputManager().getMouseY() < getYGlobal() + size.y / 2.0f + header->getHeight() / 2.0f)
						breakFromStrech = true;
				}
				else
				{//Strech began from the lower side of the window
					if (getInputManager().getMouseY() > getYGlobal() + size.y / 2.0f + header->getHeight() / 2.0f)
						breakFromStrech = true;
				}

				//Strech window based on mouse movement
				if (!breakFromStrech)
				{
					if (getInputManager().getMouseY() > position.y + size.y / 2.0f)
					{
						h += getInputManager().getMouseMovementY();
					}
					else
					{
						h -= getInputManager().getMouseMovement().y;
						if (abs(size.y - minSize.y) > 0.01f)
						translate(0, int(getInputManager().getMouseMovementY()));
					}
				}
			}

			//Set size to the calculated dimensions
			setSize(int(w), int(h));

			enableState(GUIWINDOW_LIMIT_WITHIN_MAIN_WINDOW_BIT);
		}

		//Handle strech state bit
		if (getInputEnabled() &&
			!checkState(GUIWINDOW_STRECHING_BIT) &&
			getInputManager().isKeyPressed(MOUSE_BUTTON_LEFT) &&
			mouseOverStrechArea())
		{//Begin streching

			////Define state variables
			enableState(GUIWINDOW_STRECHING_BIT);
			strechState = 0;
			//Horizontal
			if (getInputManager().getMouseX() < getXGlobal() + size.x * STRECH_CORNER_PERCENTAGE)
			{
				//Enable horizontal streching
				enableBit(strechState, STRECH_STATE_HORIZONTAL);
				//Take record that the streching began from the western side of the window
				enableBit(strechState, STRECH_STATE_W);
			}
			else if (getInputManager().getMouseX() > getXGlobal() + size.x * (1.0 - STRECH_CORNER_PERCENTAGE))
			{
				//Enable horizontal streching
				enableBit(strechState, STRECH_STATE_HORIZONTAL);
				//Take record that the streching began from the eastern side of the window
				enableBit(strechState, STRECH_STATE_E);
			}
			//Vertical
			if (getInputManager().getMouseY() < getYGlobal() + size.y * STRECH_CORNER_PERCENTAGE)
			{
				//Enable vertical streching
				enableBit(strechState, STRECH_STATE_VERTICAL);
				//Take record that the streching began from the southern side of the window
				enableBit(strechState, STRECH_STATE_S);
			}
			else if (getInputManager().getMouseY() > getYGlobal() + size.y * (1.0 - STRECH_CORNER_PERCENTAGE))
			{
				//Enable vertical streching
				enableBit(strechState, STRECH_STATE_VERTICAL);
				//Take record that the streching began from the northern side of the window
				enableBit(strechState, STRECH_STATE_N);
			}
		}
		else if (checkState(GUIWINDOW_STRECHING_BIT) &&
			(!getInputEnabled() || !getInputManager().isKeyDown(MOUSE_BUTTON_LEFT)))
		{//Conditions not met to continue streching, set to false
			disableState(GUIWINDOW_STRECHING_BIT);
			strechState = 0;
		}

		////Header double clicking
		//Timer decrease
		if (doubleClickTimer > time::Time::zero)
			doubleClickTimer -= getDeltaTimeSystem().deltaTime;
		//Check header double click
		if (getInputEnabled() && getInputManager().isKeyPressed(MOUSE_BUTTON_LEFT) && header->getMouseHover())
		{//Header has been clicked
			if (doubleClickTimer > time::Time::zero)
			{//Header double click detected
				disableState(GUIWINDOW_DRAGGING_BIT);

				if (size == minSize)
				{//Set to full window

					//Set size according to application data's window dimensions
					setSize(getWindow().getWidth() - rightBorder - leftBorder, getWindow().getHeight() - upBorder - downBorder - header->getHeight());
					setPositionLocal(0, 0);

					enableState(GUIWINDOW_LIMIT_WITHIN_MAIN_WINDOW_BIT);
				}
				else
				{//Set to min size

					//Take record of the window's old center position so that resized window can be nicely centered at the same position
					glm::vec2 oldCenterPos(getXGlobal() + size.x / 2.0f, getYGlobal() + size.y / 2.0f);

					//Rezise to min size
					setSize(minSize);

					//Reposition relative to old center position (recorded earlier)
					setPositionLocal(int(oldCenterPos.x - float(size.x) * 0.5f), int(oldCenterPos.y - float(size.y) * 0.5f));

					enableState(GUIWINDOW_LIMIT_WITHIN_MAIN_WINDOW_BIT);
				}
			}
			else
				doubleClickTimer = DOUBLE_CLICK_TIME;
		}

		//Check exit button
		if (getInputEnabled() && getInputManager().isKeyPressed(MOUSE_BUTTON_LEFT) && exit->getMouseHover() && getInputManager().tryClaimMouseAvailability())
			close();
	}

	void GUIWindow::visualUpdate()
	{
		//Perform rescaling/positioning
		GUIRectangleContainer::visualUpdate();

		//If limitation within main window is requested, do it now
		if (checkState(GUIWINDOW_LIMIT_WITHIN_MAIN_WINDOW_BIT))
		{
			limitWithinMainWindow();

			//Re-run visual update to apply limitation within main window
			GUIRectangleContainer::visualUpdate();
		}
	}

	void GUIWindow::onEnableInput()
	{
		GUIRectangleContainer::onEnableInput();
		strech->setColor(strechColorFocused);
	}

	void GUIWindow::onDisableInput()
	{
		GUIRectangleContainer::onDisableInput();
		strech->setColor(strechColorUnfocused);
	}

	bool GUIWindow::close()
	{
		if (!checkState(GUIRECT_OPEN_BIT))
			return false;
		disableState(GUIRECT_OPEN_BIT);
		disableState(GUIRECT_MOUSE_HOVER_BIT);
		disableState(GUIRECT_MOUSE_HOVER_CONTAINER_BIT);
		disableInput();
		setRenderState(false);
		return true;
	}

	bool GUIWindow::open()
	{
		if (checkState(GUIRECT_OPEN_BIT))
			return false;
		enableState(GUIRECT_OPEN_BIT);
		setRenderState(true);
		refresh();
		return true;
	}

	void GUIWindow::setPositionMode(const PositionMode mode)
	{
		if (positionMode != mode)
		{
			positionMode = mode;
			disableState(GUIRECT_MIN_SIZE_UPDATED_BIT | GUIRECT_SCALE_UPDATED_BIT | GUIRECT_POSITION_UPDATED_BIT);
		}
	}

	void GUIWindow::updateMinSize()
	{
		//Initialize min dimensions
		header->updateMinSize();
		exit->updateMinSize();
		minSize.x = header->getMinWidth() + exit->getWidth();
		minSize.y = std::max(header->getMinHeight(), exit->getMinHeight());

		//Go through every element...
		for (unsigned i = GUIWINDOW_BASE_ELEMENT_COUNT; i < elements.size(); i++)
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
		header->setHeight(std::max(header->getMinHeight(), exit->getMinHeight()));

		//Resize exit
		exit->setHeight(std::max(header->getMinHeight(), exit->getMinHeight()));

		////Resize and position elements
		if (elements.size() <= GUIWINDOW_BASE_ELEMENT_COUNT)
			return;
		if (size.y == minSize.y || positionMode == PositionMode::StackUp || positionMode == PositionMode::StackDown)
		{
			//Determine preferred reservation total
			float preferredReservation = 0;
			for (unsigned i = GUIWINDOW_BASE_ELEMENT_COUNT; i < elements.size(); i++)
				preferredReservation += elements[i]->getPreferredHeight() - elements[i]->getMinHeight();
		
			//Resize
			const int extraHeight = size.y - minSize.y;
			for (unsigned i = GUIWINDOW_BASE_ELEMENT_COUNT; i < elements.size(); i++)
			{
				const int min = elements[i]->getMinHeight();
				const int preferred = elements[i]->getPreferredHeight();
				if (min == preferred)
					elements[i]->setSize(size.x, min);
				else
				{
					const int reservation = preferred - min;
					const float percentage = float(reservation) / preferredReservation;
					elements[i]->setSize(size.x, min + int(std::min(float(reservation), std::floor(float(extraHeight) * percentage))));
				}
			}
		}
		else
		{//Use scaled size for every element

			/*
			//Determine preferred and min height
			float preferredHeight = 0;
			const float minHeight = minSize.y - header->getHeight();
			for (unsigned i = GUIWINDOW_BASE_ELEMENT_COUNT; i < elements.size(); i++)
				preferredHeight += elements[i]->getPreferredHeight();
			//Determine something
			const float availableHeight = size.y - header->getHeight();
			for (unsigned i = GUIWINDOW_BASE_ELEMENT_COUNT; i < elements.size(); i++)
			{
				const float percentage = (float)elements[i]->getPreferredHeight() / preferredHeight;
				if (int(percentage * size.y / availableHeight) < elements[i]->getMinHeight())
				{//Percentage below minimum required!

				}
			}
			*/

			int hAllocated = 0;
			int hElement = 0;
			float scalePercentage = float(size.y - header->getHeight()) / float(minSize.y - header->getMinHeight());//How many % of min size should each element scale
			for (int i = GUIWINDOW_BASE_ELEMENT_COUNT; i < (int)elements.size() - 1; i++)
			{
				hElement = int(scalePercentage * float(elements[i]->getMinHeight()));
				hAllocated += hElement;
				elements[i]->setSize(size.x, hElement);
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
		if (elements.size() <= GUIWINDOW_BASE_ELEMENT_COUNT)
			return;
		//Position first element
		elements[GUIWINDOW_BASE_ELEMENT_COUNT]->setPositionLocal(0, size.y - header->getHeight() - elements[GUIWINDOW_BASE_ELEMENT_COUNT]->getHeight());
		for (unsigned i = GUIWINDOW_BASE_ELEMENT_COUNT + 1; i < elements.size(); i++)
		{
			elements[i]->setPositionLocal(0, elements[i - 1]->getYLocal() - elements[i]->getHeight());
		}
	}

	void GUIWindow::limitWithinMainWindow()
	{//Prevent window going out of application window

		//Limit size
		if (size.y + header->getHeight() > getWindow().getHeight() - upBorder - downBorder)
		{//Window too high
			setSize(size.x, getWindow().getHeight() - header->getHeight() - upBorder - downBorder);
		}
		if (size.x > getWindow().getWidth() - leftBorder - rightBorder)
		{//Window too wide
			setSize(getWindow().getWidth() - leftBorder - rightBorder, size.y);
		}

		//////Limit position
		////Y
		if (getYGlobal() > getWindow().getHeight() - size.y - upBorder - header->getHeight())
		{//Window too up
			setPositionGlobal(position.x, getWindow().getHeight() - size.y - upBorder - header->getHeight());
		}
		else if (getYGlobal() < downBorder)
		{//Window too down
			setPositionGlobal(position.x, downBorder);
		}
		////X
		if (getXGlobal() > getWindow().getWidth() - size.x - rightBorder)
		{//Window too right
			setPositionGlobal(getWindow().getWidth() - size.x - rightBorder, position.y);
		}
		else if (getXGlobal() < leftBorder)
		{//Window too left
			setPositionGlobal(leftBorder, position.y);
		}

		updateScale();
		updatePosition();
		disableState(GUIWINDOW_LIMIT_WITHIN_MAIN_WINDOW_BIT);
	}

	void GUIWindow::setDepth(int16_t depth)
	{
		GUIRectangleContainer::setDepth(depth + 1);
		if (strech)
			strech->setDepth(depth);
	}

	bool GUIWindow::mouseOverStrechArea()
	{
		//Mouse not overlapping strech rect, no hovering
		if (!strech->getMouseHover())
			return false;

		//Check each border individually
		if (getInputManager().getMouseX() < getXGlobal())
			return true;
		if (getInputManager().getMouseY() < getYGlobal())
			return true;
		if (getInputManager().getMouseX() > getXGlobal() + size.x)
			return true;
		if (getInputManager().getMouseY() > getYGlobal() + size.y)
			return true;
		return false;
	}

	void GUIWindow::refresh()
	{
		disableBit(state, GUIRECT_MIN_SIZE_UPDATED_BIT);
		disableBit(state, GUIRECT_SCALE_UPDATED_BIT);
		disableBit(state, GUIRECT_POSITION_UPDATED_BIT);
		enableBit(state, GUIWINDOW_LIMIT_WITHIN_MAIN_WINDOW_BIT);
		refreshRequests = 0;
	}

	void GUIWindow::requestRefresh()
	{
		refreshRequests++;
	}

	bool GUIWindow::isReceivingInput() const
	{
		return isDragging() || isStreching() || GUIRectangleContainer::isReceivingInput();
	}

}