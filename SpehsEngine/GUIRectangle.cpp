#include <iostream>
#include "ApplicationData.h"
#include "Polygon.h"
#include "InputManager.h"
#include "GUIRectangle.h"
#include "TextureManager.h"
#include "GUIRectangleContainer.h"
#include "ApplicationData.h"
#include "Text.h"
#include "BatchManager.h"
#include "Depth.h"

#define TEXT_MAX_STRING_LENGTH 32

int64_t guiRectangleAllocations = 0;
int64_t guiRectangleDeallocations = 0;

namespace spehs
{
	glm::vec4 GUIRectangle::defaultColor(1.0f, 1.0f, 1.0f, 1.0f);
	void GUIRectangle::setDefaultColor(int r, int g, int b, int a)
	{
		defaultColor[0] = r / 255.0f;
		defaultColor[1] = g / 255.0f;
		defaultColor[2] = b / 255.0f;
		defaultColor[3] = a / 255.0f;
	}
	void GUIRectangle::setDefaultColor(glm::vec4 color)
	{
		defaultColor = color;
	}
	void GUIRectangle::setDefaultColor(glm::vec3 color)
	{
		memcpy(&defaultColor[0], &color[0], sizeof(float) * 3);
		defaultColor.a = 1.0f;
	}
	
	GUIRectangle::DisplayTexture::~DisplayTexture()
	{
		polygon->destroy();
	}
	GUIRectangle::GUIRectangle() : position(0), size(0), minSize(0), state(GUIRECT_ENABLED | GUIRECT_HOVER_COLOR | GUIRECT_TEXT_JUSTIFICATION_LEFT), displayTexture(nullptr), pressCallbackFunction(nullptr)
	{//Default constructor
#ifdef _DEBUG
		++guiRectangleAllocations;
#endif

		//Create polygon
		polygon = spehs::getActiveBatchManager()->createPolygon(spehs::BUTTON, GUI_PLANEDEPTH, 1.0f, 1.0f);
		polygon->setCameraMatrixState(false);
		
		setColor(defaultColor);
	}
	GUIRectangle::GUIRectangle(GUIRECT_ID_TYPE ID) : GUIRectangle()
	{
		id = ID;
	}
	GUIRectangle::GUIRectangle(std::string str) : GUIRectangle()
	{
		setString(str);
	}
	GUIRectangle::GUIRectangle(glm::ivec2& _size) : GUIRectangle()
	{
		setSize(_size);
	}
	GUIRectangle::GUIRectangle(int width, int height) : GUIRectangle()
	{
		setSize(width, height);
	}
	GUIRectangle::~GUIRectangle()
	{
#ifdef _DEBUG
		++guiRectangleDeallocations;
#endif
		if (polygon)
			polygon->destroy();
		if (text)
			text->destroy();
		if (tooltip)
			delete tooltip;
		if (displayTexture)
			delete displayTexture;
		if (pressCallbackFunction)
			delete pressCallbackFunction;
	}
	void GUIRectangle::update()
	{
		disableBit(state, GUIRECT_MOUSE_HOVER);
		disableBit(state, GUIRECT_MOUSE_HOVER_CONTAINER);
		updateMouseHover();
		if (pressCallbackFunction && getMouseHoverAny() && inputManager->isKeyPressed(MOUSEBUTTON_LEFT))
			(*pressCallbackFunction)(*this);

		//Toolti render state
		if (tooltip)
		{
			if (getMouseHover())
			{
				tooltip->setPositionLocal(inputManager->getMouseX(), inputManager->getMouseY());
				tooltip->setRenderState(true);
			}
			else
				tooltip->setRenderState(false);
		}
	}
	void GUIRectangle::postUpdate()
	{
		//Return if not visible
		if (!polygon->getRenderState())
			return;

		//Tooltip
		if (tooltip)
			tooltip->postUpdate();
		
		//Hover color
		if (checkBit(state, GUIRECT_HOVER_COLOR))
		{
			if (getMouseHover())
			{
				//Create temp vec4 col, lighten it a bit from color
				glm::vec4 col = color;
				for (unsigned i = 0; i < 3; i++)
				{
					col[i] += 0.07f;

					//Limit channel value to 1.0
					if (col[i] > 1.0f)
						col[i] = 1.0f;
				}
				polygon->setColor(col);
			}
			else
				polygon->setColor(color);
		}

		if (!checkBit(state, GUIRECT_SCALED))
		{//Rescaling
			updateScale();
			updatePosition();
		}
		else if (!checkBit(state, GUIRECT_POSITIONED))
		{//Repositioning GUI
			updatePosition();
		}
	}
	void GUIRectangle::setRenderState(const bool _state)
	{
		polygon->setRenderState(_state);
		if (text)
			text->setRenderState(_state);
		if (displayTexture)
			displayTexture->polygon->setRenderState(_state);
		if (tooltip && !_state)
			tooltip->setRenderState(false);
	}
	bool GUIRectangle::updateMouseHover()
	{
		if (inputManager->getMouseX() < getXGlobal() || inputManager->getMouseX() > getXGlobal() + size.x)
		{
			disableBit(state, GUIRECT_MOUSE_HOVER);
			return false;
		}
		else if (round(inputManager->getMouseY()) <= getYGlobal() || round(inputManager->getMouseY()) > getYGlobal() + size.y)
		{
			disableBit(state, GUIRECT_MOUSE_HOVER);
			return false;
		}
		enableBit(state, GUIRECT_MOUSE_HOVER);
		return true;
	}
	void GUIRectangle::updatePosition()
	{
		polygon->setPosition(getXGlobal() - applicationData->getWindowWidthHalf(), getYGlobal() - applicationData->getWindowHeightHalf());

		//Text position
		if (text)
		{
			float textX = getXGlobal();
			if (checkBit(state, GUIRECT_TEXT_JUSTIFICATION_LEFT))
				textX += TEXT_PREFERRED_SIZE_BORDER;
			else if (checkBit(state, GUIRECT_TEXT_JUSTIFICATION_RIGHT))
				textX += size.x - text->getTextWidth() - TEXT_PREFERRED_SIZE_BORDER;
			else
				textX += 0.5f *(size.x - text->getTextWidth());
			text->setPosition(textX, getYGlobal() + 0.5f * (size.y + text->getTextHeight()) - text->getFontHeight() - text->getFontDescender());
		}

		//Display texture position
		if (displayTexture)
		{
			displayTexture->polygon->setPosition(getXGlobal() + size.x / 2 - applicationData->getWindowWidthHalf(), getYGlobal() + size.y / 2 - applicationData->getWindowHeightHalf());
		}

		enableBit(state, GUIRECT_POSITIONED);
	}
	void GUIRectangle::updateScale()
	{
		if (!checkBit(state, GUIRECT_MIN_SIZE_UPDATED))
		{//TODO
			updateMinSize();
			enableBit(state, GUIRECT_MIN_SIZE_UPDATED);//Disable bit here because update min size function is virtual
		}

		//Account minimum size
		if (size.x < minSize.x)
			size.x = minSize.x;
		if (size.y < minSize.y)
			size.y = minSize.y;

		polygon->resize(size.x, size.y);
		enableBit(state, GUIRECT_SCALED);
	}
	void GUIRectangle::setColor(glm::vec4& c)
	{
		color = c;
		polygon->setColor(color);
	}
	void GUIRectangle::setColor(int r, int g, int b, int a)
	{
		color[0] = r / 255.0f;
		color[1] = g / 255.0f;
		color[2] = b / 255.0f;
		color[3] = a / 255.0f;
		polygon->setColor(color);
	}
	void GUIRectangle::setDepth(uint16_t depth)
	{
		polygon->setPlaneDepth(depth);
		if (text)
			text->setPlaneDepth(depth + 3);
		if (displayTexture)
			displayTexture->polygon->setPlaneDepth(depth + 1);
		if (tooltip)
			tooltip->setDepth(depth + 5000);
	}
	uint16_t GUIRectangle::getDepth()
	{
		return polygon->getPlaneDepth();
	}
	void GUIRectangle::setParent(GUIRectangle* Parent)
	{
		parent = Parent;
		setDepth(parent->getDepth());
	}
	GUIRectangle* GUIRectangle::getFirstGenerationParent()
	{
		if (parent)
			return parent->getFirstGenerationParent();
		if (getAsGUIRectangleContainerPtr())
			return getAsGUIRectangleContainerPtr();
		return nullptr;
	}
	void GUIRectangle::setJustification(GUIRECT_STATE_TYPE justificationBit)
	{
		disableBit(state, GUIRECT_TEXT_JUSTIFICATION_LEFT);
		disableBit(state, GUIRECT_TEXT_JUSTIFICATION_CENTER);
		disableBit(state, GUIRECT_TEXT_JUSTIFICATION_RIGHT);
		enableBit(state, justificationBit);
	}
	void GUIRectangle::updateMinSize()
	{
		minSize.x = 0;
		minSize.y = 0;

		if (text)
		{
			minSize.x = text->getTextWidth() + 2 * TEXT_PREFERRED_SIZE_BORDER;
			minSize.y = text->getTextHeight() + 2 * TEXT_PREFERRED_SIZE_BORDER;
		}
		if (displayTexture)
		{
			if (minSize.x < displayTexture->width)
				minSize.x = displayTexture->width;
			if (minSize.y < displayTexture->height)
				minSize.y = displayTexture->height;
		}

		if (minSize.x > size.x)
			setWidth(minSize.x);
		if (minSize.y > size.y)
			setWidth(minSize.y);
	}

	//Setters
	void GUIRectangle::setString(std::string str)
	{
		if (!text)
			createText();
		text->setString(str);

		disableStateRecursiveUpwards(GUIRECT_SCALED);
		disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED);
		disableStateRecursiveUpwards(GUIRECT_POSITIONED);
	}
	void GUIRectangle::setStringSize(int size)
	{
		if (!text)
			createText();
		else if (text->getFontSize() == size)
			return;

		text->setFontSize(size);
		disableStateRecursiveUpwards(GUIRECT_SCALED);
		disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED);
		disableStateRecursiveUpwards(GUIRECT_POSITIONED);
	}
	void GUIRectangle::setStringSizeRelative(int relativeSize)
	{
		setStringSize(applicationData->GUITextSize + relativeSize);
	}
	void GUIRectangle::setTooltip(std::string tooltipString)
	{
		//Create tooltip object if one does not exist already
		if (!tooltip)
			tooltip = new GUIRectangle();

		tooltip->setString(tooltipString);
		tooltip->setStringSize(applicationData->GUITextSize);
		tooltip->setJustification(GUIRECT_TEXT_JUSTIFICATION_LEFT);
		tooltip->setColor(150, 150, 150);
		tooltip->updateScale();
		tooltip->setSize(tooltip->minSize);
		tooltip->setRenderState(false);
	}
	void GUIRectangle::setStringColor(glm::vec4& col)
	{
		if (!text)
			createText();
		text->setColor(col); 
	}
	void GUIRectangle::setStringColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		if (!text)
			createText();
		text->setColor(r, g, b, a);
	}
	void GUIRectangle::setStringAlpha(float alpha)
	{
		if (!text)
			createText();
		text->setAlpha(alpha);
	}
	void GUIRectangle::setStringAlpha(unsigned char a)
	{
		if (!text)
			createText();
		text->setAlpha(a);
	}
	std::string GUIRectangle::getString()
	{
		if (text)
			return text->getString();
		return std::string("");
	}
	void GUIRectangle::createText()
	{
		if (text)
			return;
		text = Text::create(polygon->getPlaneDepth() + 1);
		text->setRenderState(polygon->getRenderState());
		text->setFont(applicationData->GUITextFontPath, applicationData->GUITextSize);
		text->setColor(0.0f, 0.0f, 0.0f);
	}
	void GUIRectangle::setDisplayTexture(std::string path)
	{
		if (displayTexture)
			delete displayTexture;
		displayTexture = new DisplayTexture();
		displayTexture->polygon = spehs::getActiveBatchManager()->createPolygon(4, 0, 1, 1);
		TextureData* texData = textureManager->getTextureData(path);
		displayTexture->polygon->setTexture(texData);
		displayTexture->polygon->resize(texData->width, texData->height);
		displayTexture->polygon->setCameraMatrixState(false);
		displayTexture->polygon->setRenderState(polygon->getRenderState());
		displayTexture->width = texData->width;
		displayTexture->height = texData->height;
		disableStateRecursiveUpwards(GUIRECT_POSITIONED);
		disableStateRecursiveUpwards(GUIRECT_SCALED);
	}
	void GUIRectangle::setTexture(std::string path)
	{
		polygon->setTexture(path);
	}
	bool GUIRectangle::isVisible()
	{
		return polygon->getRenderState();
	}
	void GUIRectangle::setPressCallback(std::function<void(GUIRectangle&)> callbackFunction)
	{
		if (pressCallbackFunction)
			*pressCallbackFunction = callbackFunction;
		else
			pressCallbackFunction = new std::function<void(GUIRectangle&)>(callbackFunction);
	}
}