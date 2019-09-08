#include "stdafx.h"
#include <iostream>
#include <atomic>
#include "SpehsEngine/Audio/AudioManager.h"
#include "SpehsEngine/Audio/SoundSource.h"
#include "SpehsEngine/Core/ColorUtilityFunctions.h"
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/Input/InputLib.h"
#include "SpehsEngine/Rendering/Polygon.h"
#include "SpehsEngine/GUI/GUIRectangle.h"
#include "SpehsEngine/Rendering/TextureManager.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/Window.h"
#include "SpehsEngine/GUI/GUIRectangleContainer.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/Text.h"
#include "SpehsEngine/Rendering/Depth.h"

#define TEXT_MAX_STRING_LENGTH 32

std::atomic<int> guiRectangleAllocations = 0;
std::atomic<int> guiRectangleDeallocations = 0;

namespace se
{
	int16_t GUIRectangle::defaultDepth = 10000;
	int16_t GUIRectangle::tooltipDepthRelative = 1000;
	Color GUIRectangle::defaultColor(255, 255, 255, 255);
	Color GUIRectangle::defaultStringColor(0, 0, 0, 255);
	Color GUIRectangle::defaultTooltipColor(255, 255, 255, 255);
	Color GUIRectangle::defaultTooltipStringColor(0, 0, 0, 255);
	se::rendering::TextureParameter GUIRectangle::defaultTextureParameters(se::rendering::TextureFiltering::Nearest, se::rendering::TextureFiltering::Nearest);

	GUIRectangle::DisplayTexture::~DisplayTexture()
	{
		if (polygon)
		{
			polygon->destroy();
		}
	}

	GUIRectangle::GUIRectangle(GUIContext& context)
		: GUIContext(context)
	{
#ifdef _DEBUG
		++guiRectangleAllocations;
#endif

		//Create polygon
		polygon = getBatchManager().createPolygon(se::BUTTON, defaultDepth, 1.0f, 1.0f);
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
	}

	void GUIRectangle::inputUpdate()
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

			if (getInputManager().isKeyPressed(MOUSEBUTTON_LEFT))
			{//LMB pressed
				if (pressSound)
					pressSound->play();
				if (pressCallbackFunction1)
					pressCallbackFunction1(*this);
			}

			if (getInputManager().isKeyPressed(MOUSEBUTTON_RIGHT))
			{//RMB pressed
				if (pressSound)
					pressSound->play();
				if (pressCallbackFunction2)
					pressCallbackFunction2(*this);
			}

			//Tooltip update
			if (tooltip)
			{
				int _x = int(getInputManager().getMouseX()) - tooltip->getWidth();
				int _y = int(getInputManager().getMouseY());
				if (_x < 0)
					_x = int(getInputManager().getMouseX());
				if (_x + tooltip->getWidth() > getBatchManager().window.getWidth())
					_x = getBatchManager().window.getWidth() - tooltip->getWidth();
				if (_y + tooltip->getHeight() > getBatchManager().window.getHeight())
					_y = getBatchManager().window.getHeight() - tooltip->getHeight();
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
				polygon->setColor(brightnessAddNewColor(color, 0.15f));
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
		if (getInputManager().getMouseX() < getXGlobal() || getInputManager().getMouseX() >= getXGlobal() + size.x)
		{
			disableBit(state, GUIRECT_MOUSE_HOVER_BIT);
			return false;
		}
		else if (getInputManager().getMouseY() <= getYGlobal() || getInputManager().getMouseY() >= getYGlobal() + size.y)
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
			minSize.x += int(text->getTextWidth()) + 2 * borderWidth;
			minSize.y += int(text->getTextHeight()) + 2 * borderWidth;
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

		polygon->resize(float(size.x), float(size.y));
		enableState(GUIRECT_SCALE_UPDATED_BIT);
		disableState(GUIRECT_POSITION_UPDATED_BIT);
	}

	void GUIRectangle::updatePosition()
	{
		if (!checkState(GUIRECT_SCALE_UPDATED_BIT))
			updateMinSize();
		else if (!checkState(GUIRECT_MIN_SIZE_UPDATED_BIT))
			updateMinSize();

		polygon->setPosition(float(getXGlobal()), float(getYGlobal()));

		//Text position
		float textWidth = 0.0f;
		if (text)
		{
			textWidth = text->getTextWidth();
			float left = float(getXGlobal());
			float width = float(size.x);
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
				const int border = int(std::floor((text->getX() - float(borderWidth + getXGlobal() + displayTexture->width)) * 0.5f));
				displayTexture->polygon->setPosition(float(getXGlobal() + displayTexture->width / 2 + border), float(getYGlobal() + size.y / 2));
			}
				break;
			case DisplayTexturePositionMode::right:
			{
				const int border = int(std::floor((float(getXGlobal() + size.x - displayTexture->width - borderWidth) - text->getX() - textWidth) * 0.5f));
				displayTexture->polygon->setPosition(float(getXGlobal() + size.x - displayTexture->width / 2 - border), float(getYGlobal() + size.y / 2));
			}
				break;
			case DisplayTexturePositionMode::center:
				displayTexture->polygon->setPosition(float(getXGlobal() + size.x / 2), float(getYGlobal() + size.y / 2));
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

	void GUIRectangle::setAlpha(const float a)
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
		if (tooltipString.empty())
		{
			tooltip.reset();
		}
		else
		{
			//Create tooltip object if one does not exist already
			if (!tooltip)
			{
				tooltip.reset(new GUIRectangle(getGUIContext()));
				tooltip->setStringColor(defaultTooltipStringColor);
				tooltip->setStringSize(12);
				tooltip->setColor(defaultTooltipColor);
			}

			tooltip->setString(tooltipString);
			tooltip->setJustification(GUIRECT_TEXT_JUSTIFICATION_LEFT_BIT);
			tooltip->updateScale();
			tooltip->setSize(tooltip->minSize);
			tooltip->setRenderState(false);
			tooltip->setDepth(getDepth() + tooltipDepthRelative);
		}
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

	void GUIRectangle::setStringSize(const int _size)
	{
		if (!text)
			createText();
		else if (text->getFontSize() == _size)
			return;

		text->setFontSize(_size);
		disableStateRecursiveUpwards(GUIRECT_SCALE_UPDATED_BIT);
		disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED_BIT);
		disableStateRecursiveUpwards(GUIRECT_POSITION_UPDATED_BIT);
	}

	void GUIRectangle::setStringSizeRelative(const int relativeSize)
	{
		setStringSize(12 + relativeSize);
	}

	void GUIRectangle::setStringColor(const Color& col)
	{
		if (!text)
			createText();
		text->setColor(col);
	}

	void GUIRectangle::setStringAlpha(const float alpha)
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

	const Color GUIRectangle::getStringColor() const
	{
		if (text)
		{
			return text->getColor();
		}
		else
		{
			return defaultStringColor;
		}
	}

	void GUIRectangle::createText()
	{
		if (text)
			return;
		text = getBatchManager().createText(polygon->getPlaneDepth() + 2);
		text->setRenderState(getRenderState());
		text->setFont("Fonts/Anonymous.ttf"/**/, 12);
		text->setColor(defaultStringColor);
	}

	void GUIRectangle::setDisplayTexture(const std::string& path, const se::rendering::TextureParameter& _parameters)
	{
		if (!displayTexture)
		{
			displayTexture.reset(new DisplayTexture());
		}
		se::rendering::TextureData* texData = getBatchManager().textureManager.getTextureData(path, _parameters);
		displayTexture->polygon = getBatchManager().createPolygon(4, 0, float(texData->width), float(texData->height));
		displayTexture->polygon->setTexture(texData);
		displayTexture->polygon->setCameraMatrixState(false);
		displayTexture->polygon->setRenderState(polygon->getRenderState());
		displayTexture->width = uint16_t(texData->width);
		displayTexture->height = uint16_t(texData->height);
		displayTexture->polygon->setPlaneDepth(getDepth() + 1);
		disableStateRecursiveUpwards(GUIRECT_POSITION_UPDATED_BIT | GUIRECT_SCALE_UPDATED_BIT);
	}

	void GUIRectangle::setDisplayTexture(const std::string& path)
	{
		setDisplayTexture(path, defaultTextureParameters);
	}

	void GUIRectangle::setDisplayTextureColor(const Color& _color)
	{
		if (displayTexture)
		{
			displayTexture->polygon->setColor(_color);
		}
	}

	void GUIRectangle::setTexture(const std::string& path, const se::rendering::TextureParameter& _parameters)
	{
		polygon->setTexture(getBatchManager().textureManager.getTextureData(path, _parameters));
	}

	void GUIRectangle::setTexture(const std::string& path)
	{
		setTexture(path, se::rendering::TextureParameter::defaultParameters);
	}

	void GUIRectangle::setTextureID(const unsigned int _textureID)
	{
		polygon->setTextureID(_textureID);
	}

	void GUIRectangle::setPressCallback1(const std::function<void(GUIRectangle&)>& callbackFunction)
	{
		pressCallbackFunction1 = callbackFunction;
	}

	void GUIRectangle::setPressCallback2(const std::function<void(GUIRectangle&)>& callbackFunction)
	{
		pressCallbackFunction2 = callbackFunction;
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

	void GUIRectangle::setHoverSound(const std::string& path)
	{
		if (!hoverSound)
		{
			hoverSound.reset(new se::audio::SoundSource());
		}
		hoverSound->setSound(AudioManager::load(path));
		hoverSound->setPriority(1);
		hoverSound->setGain(1.0f);
	}

	void GUIRectangle::setPressSound(const std::string& path)
	{
		if (!pressSound)
		{
			pressSound.reset(new se::audio::SoundSource());
		}
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