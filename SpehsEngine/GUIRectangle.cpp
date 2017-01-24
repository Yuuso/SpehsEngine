#include <iostream>
#include "AudioManager.h"
#include "SoundSource.h"
#include "ApplicationData.h"
#include "Polygon.h"
#include "InputManager.h"
#include "GUIRectangle.h"
#include "TextureManager.h"
#include "GUIRectangleContainer.h"
#include "ApplicationData.h"
#include "Text.h"
#include "Depth.h"

#define TEXT_MAX_STRING_LENGTH 32

int64_t guiRectangleAllocations = 0;
int64_t guiRectangleDeallocations = 0;

namespace spehs
{
	int16_t GUIRectangle::defaultDepth = 10000;
	int16_t GUIRectangle::tooltipDepthRelative = 1000;
	glm::vec4 GUIRectangle::defaultColor(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 GUIRectangle::defaultStringColor(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 GUIRectangle::defaultTooltipColor(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 GUIRectangle::defaultTooltipStringColor(0.0f, 0.0f, 0.0f, 1.0f);

	GUIRectangle::DisplayTexture::~DisplayTexture()
	{
		polygon->destroy();
	}
	GUIRectangle::GUIRectangle() : position(0), size(0), minSize(0),
		state(GUIRECT_HOVER_COLOR | GUIRECT_TEXT_JUSTIFICATION_LEFT), inputEnabled(true),
		parent(nullptr), text(nullptr), displayTexture(nullptr), tooltip(nullptr),
		pressCallbackFunction(nullptr), pressSound(nullptr), hoverSound(nullptr)
	{//Default constructor
#ifdef _DEBUG
		++guiRectangleAllocations;
#endif

		//Create polygon
		polygon = spehs::Polygon::create(spehs::BUTTON, defaultDepth, 1.0f, 1.0f);
		polygon->setCameraMatrixState(false);
		
		setColor(defaultColor);
	}
	GUIRectangle::GUIRectangle(const GUIRECT_ID_TYPE ID) : GUIRectangle()
	{
		setID(ID);
	}
	GUIRectangle::GUIRectangle(const std::string str) : GUIRectangle()
	{
		setString(str);
	}
	GUIRectangle::GUIRectangle(const glm::ivec2& _size) : GUIRectangle()
	{
		setSize(_size);
	}
	GUIRectangle::GUIRectangle(const int width, const int height) : GUIRectangle()
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
		if (pressSound)
			delete pressSound;
		if (hoverSound)
			delete hoverSound;
	}
	void GUIRectangle::inputUpdate()
	{
		disableBit(state, GUIRECT_MOUSE_HOVER);
		if (!inputEnabled)
		{
			if (tooltip)
				tooltip->setRenderState(false);
			return;
		}

		updateMouseHover();
		if (getMouseHover())
		{
			if (hoverSound)
			{//Hover sound
				if (!checkState(GUIRECT_MOUSE_HOVER_PREVIOUS))
					hoverSound->play();
			}

			if (inputManager->isKeyPressed(MOUSEBUTTON_LEFT))
			{//Pressed

				if (pressSound)
					pressSound->play();

				if (pressCallbackFunction)
					(*pressCallbackFunction)(*this);
			}

			//Tooltip update
			if (tooltip)
			{
				int _x = inputManager->getMouseX() - tooltip->getWidth(), _y = inputManager->getMouseY();
				if (_x < 0)
					_x = inputManager->getMouseX();
				if (_x + tooltip->getWidth() > applicationData->getWindowWidth())
					_x = applicationData->getWindowWidth() - tooltip->getWidth();
				if (_y + tooltip->getHeight() > applicationData->getWindowHeight())
					_y = applicationData->getWindowHeight() - tooltip->getHeight();
				tooltip->setPositionGlobal(_x, _y);
				tooltip->setRenderState(true);
			}

			enableState(GUIRECT_MOUSE_HOVER_PREVIOUS);
		}
		else
		{//No mouse hover
			if (tooltip)
				tooltip->setRenderState(false);
			disableState(GUIRECT_MOUSE_HOVER_PREVIOUS);
		}
	}
	void GUIRectangle::visualUpdate()
	{
		//Return if not visible
		if (!getRenderState())
			return;

		//Tooltip
		if (tooltip)
			tooltip->visualUpdate();
		
		//Hover color
		if (checkState(GUIRECT_HOVER_COLOR) && inputEnabled)
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
		else
			polygon->setColor(color);

		if (!checkState(GUIRECT_MIN_SIZE_UPDATED_BIT))
		{//Recalculate min size + rescale + reposition
			updateMinSize(); enableState(GUIRECT_MIN_SIZE_UPDATED_BIT);
			updateScale(); enableState(GUIRECT_SCALE_UPDATED_BIT);
			updatePosition(); enableState(GUIRECT_POSITION_UPDATED_BIT);
		}
		else if (!checkBit(state, GUIRECT_SCALE_UPDATED_BIT))
		{//Rescale + reposition
			updateScale(); enableState(GUIRECT_SCALE_UPDATED_BIT);
			updatePosition(); enableState(GUIRECT_POSITION_UPDATED_BIT);
		}
		else if (!checkBit(state, GUIRECT_POSITION_UPDATED_BIT))
		{//Reposition
			updatePosition(); enableState(GUIRECT_POSITION_UPDATED_BIT);
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
	bool GUIRectangle::getRenderState() const
	{
		return polygon->getRenderState();
	}
	bool GUIRectangle::updateMouseHover()
	{
		if (inputManager->getMouseX() < getXGlobal() || inputManager->getMouseX() >= getXGlobal() + size.x)
		{
			disableBit(state, GUIRECT_MOUSE_HOVER);
			return false;
		}
		else if (inputManager->getMouseY() <= getYGlobal() || inputManager->getMouseY() >= getYGlobal() + size.y)
		{
			disableBit(state, GUIRECT_MOUSE_HOVER);
			return false;
		}
		enableBit(state, GUIRECT_MOUSE_HOVER);
		return true;
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

		enableState(GUIRECT_MIN_SIZE_UPDATED_BIT);
		disableState(GUIRECT_SCALE_UPDATED_BIT);
	}
	void GUIRectangle::updateScale()
	{
		if (!checkState(GUIRECT_MIN_SIZE_UPDATED_BIT))
			updateMinSize();

		//Account minimum size
		if (size.x < minSize.x)
			size.x = minSize.x;
		if (size.y < minSize.y)
			size.y = minSize.y;

		polygon->resize(size.x, size.y);
		enableState(GUIRECT_SCALE_UPDATED_BIT);
		disableState(GUIRECT_POSITION_UPDATED_BIT);
	}
	void GUIRectangle::updatePosition()
	{
		if (!checkState(GUIRECT_SCALE_UPDATED_BIT))
			updateMinSize();
		else if (!checkState(GUIRECT_MIN_SIZE_UPDATED_BIT))
			updateMinSize();

		polygon->setPosition(getXGlobal(), getYGlobal());

		//Text position
		if (text)
		{
			float textX(getXGlobal());
			if (checkBit(state, GUIRECT_TEXT_JUSTIFICATION_LEFT))
				textX += TEXT_PREFERRED_SIZE_BORDER;
			else if (checkBit(state, GUIRECT_TEXT_JUSTIFICATION_RIGHT))
				textX += size.x - text->getTextWidth() - TEXT_PREFERRED_SIZE_BORDER;
			else
				textX += 0.5f * (size.x - text->getTextWidth());
			text->setPosition(std::round(textX), std::round(getYGlobal() + 0.5f * size.y - 0.5f * text->getTextHeight() - text->getFontDescender()));
		}

		//Display texture position
		if (displayTexture)
		{
			displayTexture->polygon->setPosition(getXGlobal() + size.x / 2, getYGlobal() + size.y / 2);
		}

		enableState(GUIRECT_POSITION_UPDATED_BIT);
	}
	void GUIRectangle::setColor(const glm::vec3& c)
	{
		color.r = c.r;
		color.g = c.g;
		color.b = c.b;
		color.a = 1.0f;
		polygon->setColor(color);
	}
	void GUIRectangle::setColor(const glm::vec4& c)
	{
		color = c;
		polygon->setColor(color);
	}
	void GUIRectangle::setColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
	{
		color[0] = r / 255.0f;
		color[1] = g / 255.0f;
		color[2] = b / 255.0f;
		color[3] = a / 255.0f;
		polygon->setColor(color);
	}
	void GUIRectangle::setDepth(const int16_t depth)
	{
		polygon->setPlaneDepth(depth);
		if (displayTexture)
			displayTexture->polygon->setPlaneDepth(depth + 1);
		if (text)
			text->setPlaneDepth(depth + 2);
		if (tooltip)
			tooltip->setDepth(depth + tooltipDepthRelative);
	}
	int16_t GUIRectangle::getDepth() const
	{
		return polygon->getPlaneDepth();
	}
	void GUIRectangle::setParent(GUIRectangleContainer* Parent)
	{
		parent = Parent;
		setDepth(parent->getDepth() + 3);
	}
	GUIRectangleContainer* GUIRectangle::getFirstGenerationParent()
	{
		if (parent)
			return parent->getFirstGenerationParent();
		if (getAsGUIRectangleContainerPtr())
			return getAsGUIRectangleContainerPtr();
		return nullptr;
	}
	bool GUIRectangle::isDescendantOf(GUIRectangleContainer* ascendant) const
	{
		if (parent)
		{
			if (parent == ascendant)
				return true;
			return parent->isDescendantOf(ascendant);
		}
		return false;
	}
	void GUIRectangle::setJustification(const GUIRECT_STATE_TYPE justificationBit)
	{
		disableBit(state, GUIRECT_TEXT_JUSTIFICATION_LEFT);
		disableBit(state, GUIRECT_TEXT_JUSTIFICATION_CENTER);
		disableBit(state, GUIRECT_TEXT_JUSTIFICATION_RIGHT);
		enableBit(state, justificationBit);
	}

	//Setters
	void GUIRectangle::setTooltip(const std::string tooltipString)
	{
		//Create tooltip object if one does not exist already
		if (!tooltip)
		{
			tooltip = new GUIRectangle();
			tooltip->setStringColor(defaultTooltipStringColor);
			tooltip->setStringSize(applicationData->GUITextSize);
			tooltip->setColor(defaultTooltipColor);
		}

		tooltip->setString(tooltipString);
		tooltip->setJustification(GUIRECT_TEXT_JUSTIFICATION_LEFT);
		tooltip->updateScale();
		tooltip->setSize(tooltip->minSize);
		tooltip->setRenderState(false);
		tooltip->setDepth(getDepth() + tooltipDepthRelative);
	}
	void GUIRectangle::setString(const std::string str)
	{
		if (str.size() == 0)
			return;
		if (!text)
			createText();
		text->setString(str);

		disableStateRecursiveUpwards(GUIRECT_SCALE_UPDATED_BIT);
		disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED_BIT);
		disableStateRecursiveUpwards(GUIRECT_POSITION_UPDATED_BIT);
	}
	void GUIRectangle::setStringSize(const int size)
	{
		if (!text)
			createText();
		else if (text->getFontSize() == size)
			return;

		text->setFontSize(size);
		disableStateRecursiveUpwards(GUIRECT_SCALE_UPDATED_BIT);
		disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED_BIT);
		disableStateRecursiveUpwards(GUIRECT_POSITION_UPDATED_BIT);
	}
	void GUIRectangle::setStringSizeRelative(const int relativeSize)
	{
		setStringSize(applicationData->GUITextSize + relativeSize);
	}
	void GUIRectangle::setStringColor(const glm::vec4& col)
	{
		if (!text)
			createText();
		text->setColor(col); 
	}
	void GUIRectangle::setStringColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
	{
		if (!text)
			createText();
		text->setColor(r, g, b, a);
	}
	void GUIRectangle::setStringAlpha(const float alpha)
	{
		if (!text)
			createText();
		text->setAlpha(alpha);
	}
	void GUIRectangle::setStringAlpha(const unsigned char a)
	{
		if (!text)
			createText();
		text->setAlpha(a);
	}
	std::string GUIRectangle::getString() const
	{
		if (text)
			return text->getString();
		return "";
	}
	void GUIRectangle::createText()
	{
		if (text)
			return;
		text = Text::create(polygon->getPlaneDepth() + 1);
		text->setRenderState(getRenderState());
		text->setFont(applicationData->GUITextFontPath, applicationData->GUITextSize);
		text->setColor(defaultStringColor);
	}
	void GUIRectangle::setDisplayTexture(const std::string path)
	{
		if (displayTexture)
			delete displayTexture;
		displayTexture = new DisplayTexture();
		displayTexture->polygon = spehs::Polygon::create(4, 0, 1, 1);
		TextureData* texData = textureManager->getTextureData(path);
		displayTexture->polygon->setTexture(texData);
		displayTexture->polygon->resize(texData->width, texData->height);
		displayTexture->polygon->setCameraMatrixState(false);
		displayTexture->polygon->setRenderState(polygon->getRenderState());
		displayTexture->width = texData->width;
		displayTexture->height = texData->height;
		disableStateRecursiveUpwards(GUIRECT_POSITION_UPDATED_BIT);
		disableStateRecursiveUpwards(GUIRECT_SCALE_UPDATED_BIT);
	}
	void GUIRectangle::setTexture(const std::string path)
	{
		polygon->setTexture(path);
	}
	void GUIRectangle::setPressCallback(std::function<void(GUIRectangle&)> callbackFunction)
	{
		if (pressCallbackFunction)
			*pressCallbackFunction = callbackFunction;
		else
			pressCallbackFunction = new std::function<void(GUIRectangle&)>(callbackFunction);
	}
	void GUIRectangle::enableStateRecursiveUpwards(const GUIRECT_STATE_TYPE stateBit)
	{
		enableState(stateBit);
		if (parent)
			parent->enableStateRecursiveUpwards(stateBit);
	}
	void GUIRectangle::disableStateRecursiveUpwards(const GUIRECT_STATE_TYPE stateBit)
	{
		disableState(stateBit);
		if (parent)
			parent->disableStateRecursiveUpwards(stateBit);
	}
	glm::ivec2 GUIRectangle::getPositionGlobal() const
	{
		if (parent)
			return parent->getPositionGlobal() + position; return position;
	}
	int GUIRectangle::getXGlobal() const
	{
		if (parent)
			return parent->getXGlobal() + position.x; return position.x;
	}
	int GUIRectangle::getYGlobal() const
	{
		if (parent)
			return parent->getYGlobal() + position.y; return position.y;
	}
	void GUIRectangle::setHoverSound(const std::string path)
	{
		if (!hoverSound)
			hoverSound = new spehs::audio::SoundSource();
		hoverSound->setSound(AudioManager::instance->loadWAVE(path));
		hoverSound->setPriority(1);
		hoverSound->setGain(1.0f);
	}
	void GUIRectangle::setPressSound(const std::string path)
	{
		if (!pressSound)
			pressSound = new spehs::audio::SoundSource();
		pressSound->setSound(AudioManager::instance->loadWAVE(path));
		pressSound->setPriority(1);
		pressSound->setGain(1.0f);
	}
	void GUIRectangle::setWidth(const int width)
	{
		if (size.x == width)
			return;
		size.x = width;
		disableStateRecursiveUpwards(GUIRECT_SCALE_UPDATED_BIT);
	}
	void GUIRectangle::setHeight(const int height)
	{
		if (size.y == height)
			return;
		size.y = height;
		disableStateRecursiveUpwards(GUIRECT_SCALE_UPDATED_BIT);
	}
}