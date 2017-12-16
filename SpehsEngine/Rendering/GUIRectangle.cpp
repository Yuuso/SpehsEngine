#include <iostream>
#include <atomic>
#include "SpehsEngine/Audio/AudioManager.h"
#include "SpehsEngine/Audio/SoundSource.h"
#include "SpehsEngine/Core/ApplicationData.h"
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/Rendering/Polygon.h"
#include "SpehsEngine/Rendering/GUIRectangle.h"
#include "SpehsEngine/Rendering/TextureManager.h"
#include "SpehsEngine/Rendering/GUIRectangleContainer.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/Text.h"
#include "SpehsEngine/Rendering/Depth.h"

#define TEXT_MAX_STRING_LENGTH 32

std::atomic<int> guiRectangleAllocations = 0;
std::atomic<int> guiRectangleDeallocations = 0;

namespace spehs
{
	int16_t GUIRectangle::defaultDepth = 10000;
	int16_t GUIRectangle::tooltipDepthRelative = 1000;
	Color GUIRectangle::defaultColor(255, 255, 255, 255);
	Color GUIRectangle::defaultStringColor(0, 0, 0, 255);
	Color GUIRectangle::defaultTooltipColor(255, 255, 255, 255);
	Color GUIRectangle::defaultTooltipStringColor(0, 0, 0, 255);
	spehs::TextureParameter GUIRectangle::defaultTextureParameters(spehs::TextureFiltering::Nearest, spehs::TextureFiltering::Nearest);

	GUIRectangle::DisplayTexture::~DisplayTexture()
	{
		polygon->destroy();
	}
	GUIRectangle::GUIRectangle(BatchManager& _batchManager)
		: batchManager(_batchManager)
		, position(0, 0)
		, size(0, 0)
		, minSize(0, 0)
		, state(GUIRECT_HOVER_COLOR_BIT | GUIRECT_TEXT_JUSTIFICATION_LEFT_BIT)
		, inputEnabled(true)
		, parent(nullptr)
		, text(nullptr)
		, displayTexture(nullptr)
		, tooltip(nullptr)
		, pressCallbackFunction(nullptr)
		, pressSound(nullptr)
		, hoverSound(nullptr)
		, displayTexturePositionMode(DisplayTexturePositionMode::center)
		, borderWidth(2)
	{//Default constructor
#ifdef _DEBUG
		++guiRectangleAllocations;
#endif

		//Create polygon
		polygon = batchManager.createPolygon(spehs::BUTTON, defaultDepth, 1.0f, 1.0f);
		polygon->setCameraMatrixState(false);
		
		setColor(defaultColor);
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

	void GUIRectangle::inputUpdate(InputUpdateData& data)
	{
		disableBit(state, GUIRECT_MOUSE_HOVER_BIT);
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
				if (!checkState(GUIRECT_MOUSE_HOVER_PREVIOUS_BIT))
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
				if (_x + tooltip->getWidth() > spehs::ApplicationData::getWindowWidth())
					_x = spehs::ApplicationData::getWindowWidth() - tooltip->getWidth();
				if (_y + tooltip->getHeight() > spehs::ApplicationData::getWindowHeight())
					_y = spehs::ApplicationData::getWindowHeight() - tooltip->getHeight();
				tooltip->setPositionGlobal(_x, _y);
				tooltip->setRenderState(true);
			}

			enableState(GUIRECT_MOUSE_HOVER_PREVIOUS_BIT);
		}
		else
		{//No mouse hover
			if (tooltip)
				tooltip->setRenderState(false);
			disableState(GUIRECT_MOUSE_HOVER_PREVIOUS_BIT);
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
		if (checkState(GUIRECT_HOVER_COLOR_BIT) && inputEnabled)
		{
			if (getMouseHover())
			{
				//Create temp vec4 col, lighten it a bit from color
				Color col = color;
				col.brightness(1.1f);
				col.r += 13;
				col.g += 13;
				col.b += 13;
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
		//disableState(GUIRECT_MOUSE_HOVER_BIT | GUIRECT_MOUSE_HOVER_CONTAINER_BIT);
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
			disableBit(state, GUIRECT_MOUSE_HOVER_BIT);
			return false;
		}
		else if (inputManager->getMouseY() <= getYGlobal() || inputManager->getMouseY() >= getYGlobal() + size.y)
		{
			disableBit(state, GUIRECT_MOUSE_HOVER_BIT);
			return false;
		}
		enableBit(state, GUIRECT_MOUSE_HOVER_BIT);
		return true;
	}

	void GUIRectangle::updateMinSize()
	{
		minSize.x = 0;
		minSize.y = 0;
		
		if (text)
		{//Text
			minSize.x += text->getTextWidth() + 2 * borderWidth;
			minSize.y += text->getTextHeight() + 2 * borderWidth;
		}

		if (displayTexture)
		{//Display texture
			if (displayTexturePositionMode == DisplayTexturePositionMode::center)
			{//Overlaps with text
				if (minSize.x < displayTexture->width)
					minSize.x += displayTexture->width;
			}
			else
				minSize.x += displayTexture->width;//Separate from text
			if (minSize.y < displayTexture->height)
				minSize.y = displayTexture->height;
		}

		if (text || displayTexture)
		{//Add border
			minSize.x += 2 * borderWidth;
			minSize.y += 2 * borderWidth;
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
		float textWidth = 0.0f;
		if (text)
		{
			textWidth = text->getTextWidth();
			float left = getXGlobal();
			float width = size.x;
			if (displayTexture)
			{
				if (displayTexturePositionMode != DisplayTexturePositionMode::center)
					width -= displayTexture->width;
				if (displayTexturePositionMode == DisplayTexturePositionMode::left)
					left += displayTexture->width;
			}

			float textX = left;
			if (checkBit(state, GUIRECT_TEXT_JUSTIFICATION_LEFT_BIT))
			{//LEFT
				textX += borderWidth;
			}
			else if (checkBit(state, GUIRECT_TEXT_JUSTIFICATION_RIGHT_BIT))
			{//RIGHT
				textX += width - borderWidth - textWidth;
			}
			else
			{//CENTER
				textX += 0.5f * width - 0.5f * textWidth;
			}
			text->setPosition(std::round(textX), std::round(getYGlobal() + 0.5f * size.y - 0.5f * text->getTextHeight() - text->getFontDescender()));
			textWidth += 2 * borderWidth;
		}

		//Display texture position
		if (displayTexture)
		{
			switch (displayTexturePositionMode)
			{
			case DisplayTexturePositionMode::left:
			{
				const int border = std::floor((text->getX() - borderWidth - getXGlobal() - displayTexture->width) * 0.5f);
				displayTexture->polygon->setPosition(getXGlobal() + displayTexture->width / 2 + border, getYGlobal() + size.y / 2);
			}
				break;
			case DisplayTexturePositionMode::right:
			{
				const int border = std::floor((getXGlobal() + size.x - text->getX() - textWidth - borderWidth - displayTexture->width) * 0.5f);
				displayTexture->polygon->setPosition(getXGlobal() + size.x - displayTexture->width / 2 - border, getYGlobal() + size.y / 2);
			}
				break;
			case DisplayTexturePositionMode::center:
				displayTexture->polygon->setPosition(getXGlobal() + size.x / 2, getYGlobal() + size.y / 2);
				break;
			}
		}

		enableState(GUIRECT_POSITION_UPDATED_BIT);
	}

	void GUIRectangle::setColor(const Color& c)
	{
		color = c;
		polygon->setColor(c);
	}

	void GUIRectangle::setAlpha(const Color::Component& a)
	{
		color.a = a;
		polygon->setAlpha(a);
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
		setDepth(parent->getDepth() + 10);
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
		disableBit(state, GUIRECT_TEXT_JUSTIFICATION_LEFT_BIT);
		disableBit(state, GUIRECT_TEXT_JUSTIFICATION_CENTER_BIT);
		disableBit(state, GUIRECT_TEXT_JUSTIFICATION_RIGHT_BIT);
		enableBit(state, justificationBit);
	}

	void GUIRectangle::setTooltip(const std::string& tooltipString)
	{
		//Create tooltip object if one does not exist already
		if (!tooltip)
		{
			tooltip = new GUIRectangle(batchManager);
			tooltip->setStringColor(defaultTooltipStringColor);
			tooltip->setStringSize(spehs::ApplicationData::GUITextSize);
			tooltip->setColor(defaultTooltipColor);
		}

		tooltip->setString(tooltipString);
		tooltip->setJustification(GUIRECT_TEXT_JUSTIFICATION_LEFT_BIT);
		tooltip->updateScale();
		tooltip->setSize(tooltip->minSize);
		tooltip->setRenderState(false);
		tooltip->setDepth(getDepth() + tooltipDepthRelative);
	}

	void GUIRectangle::setString(const std::string& str)
	{
		if (str.size() == 0)
		{
			if (text)
				text->destroy();
			text = nullptr;
			return;
		}
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
		setStringSize(spehs::ApplicationData::GUITextSize + relativeSize);
	}

	void GUIRectangle::setStringColor(const Color& col)
	{
		if (!text)
			createText();
		text->setColor(col);
	}

	void GUIRectangle::setStringAlpha(const Color::Component& alpha)
	{
		if (!text)
			createText();
		text->setAlpha(alpha);
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
		text = batchManager.createText(polygon->getPlaneDepth() + 2);
		text->setRenderState(getRenderState());
		text->setFont(spehs::ApplicationData::GUITextFontPath, spehs::ApplicationData::GUITextSize);
		text->setColor(defaultStringColor);
	}

	void GUIRectangle::setDisplayTexture(const std::string& path, const TextureParameter& _parameters)
	{
		if (displayTexture)
			delete displayTexture;
		displayTexture = new DisplayTexture();
		TextureData* texData = textureManager->getTextureData(path, _parameters);
		displayTexture->polygon = batchManager.createPolygon(4, 0, texData->width, texData->height);
		displayTexture->polygon->setTexture(texData);
		displayTexture->polygon->setCameraMatrixState(false);
		displayTexture->polygon->setRenderState(polygon->getRenderState());
		displayTexture->width = texData->width;
		displayTexture->height = texData->height;
		displayTexture->polygon->setPlaneDepth(getDepth() + 1);
		disableStateRecursiveUpwards(GUIRECT_POSITION_UPDATED_BIT | GUIRECT_SCALE_UPDATED_BIT);
	}

	void GUIRectangle::setDisplayTexture(const std::string& path)
	{
		setDisplayTexture(path, defaultTextureParameters);
	}

	void GUIRectangle::setDisplayTextureColor(const Color& color)
	{
		if (displayTexture)
			displayTexture->polygon->setColor(color);
	}

	void GUIRectangle::setTexture(const std::string& path, const TextureParameter& _parameters)
	{
		polygon->setTexture(textureManager->getTextureData(path, _parameters));
	}

	void GUIRectangle::setTexture(const std::string& path)
	{
		setTexture(path, spehs::TextureParameter::defaultParameters);
	}

	void GUIRectangle::setTextureID(const unsigned int _textureID)
	{
		polygon->setTextureID(_textureID);
	}

	void GUIRectangle::setPressCallback(const std::function<void(GUIRectangle&)> callbackFunction)
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

	spehs::ivec2 GUIRectangle::getPositionGlobal() const
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

	void GUIRectangle::setHoverSound(const std::string& path)
	{
		if (!hoverSound)
			hoverSound = new spehs::audio::SoundSource();
		hoverSound->setSound(AudioManager::load(path));
		hoverSound->setPriority(1);
		hoverSound->setGain(1.0f);
	}

	void GUIRectangle::setPressSound(const std::string& path)
	{
		if (!pressSound)
			pressSound = new spehs::audio::SoundSource();
		pressSound->setSound(AudioManager::load(path));
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

	void GUIRectangle::setBorderWidth(const int w)
	{
		borderWidth = w;
		disableState(GUIRECT_MIN_SIZE_UPDATED_BIT | GUIRECT_SCALE_UPDATED_BIT | GUIRECT_POSITION_UPDATED_BIT);
	}
}