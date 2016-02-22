#include "ApplicationData.h"
#include "PolygonBatch.h"
#include "InputManager.h"
#include "GUIRectangle.h"
#include "TextureManager.h"
#include "GUIRectangleContainer.h"
#include "ApplicationData.h"
#include "Text.h"
#define TEXT_MAX_STRING_LENGTH 32
int64_t guiRectangleAllocations = 0;
int64_t guiRectangleDeallocations = 0;

namespace SpehsEngine
{
	GUIRectangle::DisplayTexture::~DisplayTexture()
	{
		delete polygon;
	}
	GUIRectangle::GUIRectangle() : position(0), size(0), minSize(0), state(0), displayTexture(nullptr)
	{//Default constructor
#ifdef _DEBUG
		++guiRectangleAllocations;
#endif

		//Create polygon
		polygon = new SpehsEngine::PolygonBatch(SpehsEngine::Shape(SpehsEngine::BUTTON), 1, 1);
		polygon->setCameraMatrixState(false);

		//Initial state (0)
		enableBit(state, GUIRECT_ENABLED);
		enableBit(state, GUIRECT_VISIBLE);
		enableBit(state, GUIRECT_HOVER_COLOR);

		setColor(125, 125, 125);
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
		delete polygon;
		if (text)
			delete text;
		if (tooltip)
			delete tooltip;
		if (displayTexture)
			delete displayTexture;
	}
	void GUIRectangle::update()
	{
		updateMouseHover();
	}
	void GUIRectangle::render()
	{
		//Hover color
		if (checkBit(state, GUIRECT_HOVER_COLOR	))
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
		{//Rescaling GUI
			updateScale();
			updatePosition();
		}
		else if (!checkBit(state, GUIRECT_POSITIONED))
		{//Repositioning GUI
			updatePosition();
		}

		//Drawing rectangle polygon
		if (checkBit(state, GUIRECT_VISIBLE))
		{
			polygon->draw();

			//Rendering text
			if (text)
				text->render();

			//Rendering display texture
			if (displayTexture)
				displayTexture->polygon->draw();

		}
	}
	void GUIRectangle::postRender()
	{
		//Rendering tooltip if one exists
		if (tooltip && getMouseHover())
		{
			tooltip->setPosition(inputManager->getMouseX(), inputManager->getMouseY());
			tooltip->render();
		}
		disableBit(state, GUIRECT_MOUSE_HOVER);
		disableBit(state, GUIRECT_MOUSE_HOVER_CONTAINER);
	}
	bool GUIRectangle::updateMouseHover()
	{
		if (inputManager->getMouseX() < getX() || inputManager->getMouseX() > getX() + size.x)
		{
			disableBit(state, GUIRECT_MOUSE_HOVER);
			return false;
		}
		else if (inputManager->getMouseY() < getY() || inputManager->getMouseY() > getY() + size.y)
		{
			disableBit(state, GUIRECT_MOUSE_HOVER);
			return false;
		}
		else
		{
			enableBit(state, GUIRECT_MOUSE_HOVER);
			return true;
		}
	}
	void GUIRectangle::updatePosition()
	{
		polygon->setPosition(getX() - applicationData->getWindowWidthHalf(), getY() - applicationData->getWindowHeightHalf());

		//Text position
		if (text)
		{
			float textX = getX();
			if (checkBit(state, GUIRECT_TEXT_JUSTIFICATION_LEFT))
				textX += TEXT_PREFERRED_SIZE_BORDER;
			else if (checkBit(state, GUIRECT_TEXT_JUSTIFICATION_RIGHT))
				textX += size.x - text->getTextWidth() - TEXT_PREFERRED_SIZE_BORDER;
			else
				textX += 0.5f *(size.x - text->getTextWidth());
			text->setPosition(textX, getY() + 0.5f * (size.y + text->getTextHeight()) - text->getFontHeight() - text->getFontDescender());
		}

		//Display texture position
		if (displayTexture)
		{
			displayTexture->polygon->setPosition(getX() + size.x / 2 - applicationData->getWindowWidthHalf(), getY() + size.y / 2 - applicationData->getWindowHeightHalf());
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
	void GUIRectangle::setTooltip(std::string tooltipString)
	{
		//Create tooltip object if one does not exist already
		if (!tooltip)
			tooltip = new GUIRectangle();

		tooltip->setString(tooltipString);
		tooltip->setStringSize(applicationData->GUITextSize);
		tooltip->setJustification(GUIRECT_TEXT_JUSTIFICATION_CENTER);
		tooltip->setColor(150, 150, 150);
		tooltip->updateScale();
		tooltip->setSize(tooltip->minSize);
	}
	void GUIRectangle::setStringColor(glm::vec4& col)
	{
		if (!text)
			createText();
		text->setColor(col); 
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
		text = new SpehsEngine::Text();
		text->setFont(applicationData->GUITextFontPath, applicationData->GUITextSize);
		text->setColor(0.05f, 0.05f, 0.05f);
	}
	void GUIRectangle::setDisplayTexture(std::string path)
	{
		if (displayTexture)
			delete displayTexture;
		displayTexture = new DisplayTexture();
		displayTexture->polygon = new PolygonBatch(4, 1, 1);
		displayTexture->polygon->setTexture(path);
		TextureSizeData* texSize = textureManager->getSizeData(path);
		displayTexture->polygon->resize(texSize->width, texSize->height);
		displayTexture->polygon->setCameraMatrixState(false);
		displayTexture->width = texSize->width;
		displayTexture->height = texSize->height;
	}
	void GUIRectangle::setTexture(std::string path)
	{
		polygon->setTexture(path);
	}

}