#include <algorithm>
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/Rendering/GUIColorEditor.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/Polygon.h"

namespace spehs
{
	float GUIColorEditor::paletteWidth = 100;
	float GUIColorEditor::sliderWidth = 10;
	int GUIColorEditor::colorEditorBorder = 3;
	float GUIColorEditor::colorPreviewWidth = 20.0f;

	GUIColorEditor::GUIColorEditor(BatchManager& _batchManager)
		: GUIRectangle(_batchManager)
		, ValueEditor(Color(255,255,255,255))
		, sliderState(0.0f)
		, sliderOnHold(false)
		, paletteOnHold(false)
	{
		palette = batchManager.createPolygon(Shape::BUTTON, getDepth(), 1.0f, 1.0f);
		sliderRG = batchManager.createPolygon(Shape::BUTTON, getDepth(), 1.0f, 1.0f);
		sliderGB = batchManager.createPolygon(Shape::BUTTON, getDepth(), 1.0f, 1.0f);
		sliderBR = batchManager.createPolygon(Shape::BUTTON, getDepth(), 1.0f, 1.0f);
		preview = batchManager.createPolygon(6, getDepth(), colorPreviewWidth, colorPreviewWidth);
		alphaSliderBack = batchManager.createPolygon(Shape::BUTTON, getDepth(), 1.0f, 1.0f);
		alphaSliderFront = batchManager.createPolygon(Shape::BUTTON, getDepth(), 1.0f, 1.0f);

		palette->setCameraMatrixState(false);
		sliderRG->setCameraMatrixState(false);
		sliderGB->setCameraMatrixState(false);
		sliderBR->setCameraMatrixState(false);
		preview->setCameraMatrixState(false);
		alphaSliderBack->setCameraMatrixState(false);
		alphaSliderFront->setCameraMatrixState(false);
		
		//Palette colors
		palette->worldVertexArray[0].color = spehs::Color(0, 0, 0, 255);
		palette->worldVertexArray[1].color = spehs::Color(0, 0, 0, 255);
		palette->worldVertexArray[2].color = spehs::Color(0, 255, 255, 255);
		palette->worldVertexArray[3].color = spehs::Color(255, 255, 255, 255);

		//Slider colors
		//Red-Green
		sliderRG->worldVertexArray[0].color = spehs::Color(255, 0, 0, 255);
		sliderRG->worldVertexArray[1].color = spehs::Color(255, 0, 0, 255);
		sliderRG->worldVertexArray[2].color = spehs::Color(0, 255, 0, 255);
		sliderRG->worldVertexArray[3].color = spehs::Color(0, 255, 0, 255);
		//Green-blue
		sliderGB->worldVertexArray[0].color = spehs::Color(0, 255, 0, 255);
		sliderGB->worldVertexArray[1].color = spehs::Color(0, 255, 0, 255);
		sliderGB->worldVertexArray[2].color = spehs::Color(0, 0, 255, 255);
		sliderGB->worldVertexArray[3].color = spehs::Color(0, 0, 255, 255);
		//Blue-red
		sliderBR->worldVertexArray[0].color = spehs::Color(0, 0, 255, 255);
		sliderBR->worldVertexArray[1].color = spehs::Color(0, 0, 255, 255);
		sliderBR->worldVertexArray[2].color = spehs::Color(255, 0, 0, 255);
		sliderBR->worldVertexArray[3].color = spehs::Color(255, 0, 0, 255);

		//Alpha slider colors
		alphaSliderBack->setColor(spehs::Color(0, 0, 0, 255));
		alphaSliderFront->setColor(spehs::Color(255, 255, 255, 255));

		setDepth(getDepth());
		setRenderState(getRenderState());
		disableState(GUIRECT_HOVER_COLOR_BIT);

		onEditorValueChange();
	}
	GUIColorEditor::~GUIColorEditor()
	{
		palette->destroy();
		sliderRG->destroy();
		sliderGB->destroy();
		sliderBR->destroy();
		preview->destroy();
		alphaSliderBack->destroy();
		alphaSliderFront->destroy();
	}
	void GUIColorEditor::setDepth(const int16_t _depth)
	{
		GUIRectangle::setDepth(_depth);
		palette->setPlaneDepth(_depth + 1);
		sliderRG->setPlaneDepth(_depth + 1);
		sliderGB->setPlaneDepth(_depth + 1);
		sliderBR->setPlaneDepth(_depth + 1);
		preview->setPlaneDepth(_depth + 2);
		alphaSliderBack->setPlaneDepth(_depth + 1);
		alphaSliderFront->setPlaneDepth(_depth + 2);
	}
	void GUIColorEditor::setRenderState(const bool _state)
	{
		GUIRectangle::setRenderState(_state);
		palette->setRenderState(_state);
		sliderRG->setRenderState(_state);
		sliderGB->setRenderState(_state);
		sliderBR->setRenderState(_state);
		preview->setRenderState(false);
		alphaSliderBack->setRenderState(_state && alphaEnabled);
		alphaSliderFront->setRenderState(_state && alphaEnabled);
	}
	void GUIColorEditor::inputUpdate(InputUpdateData& data)
	{
		GUIRectangle::inputUpdate(data);
		
		if (getInputEnabled())
		{
			//Palette
			if (paletteOnHold && !inputManager->isKeyDown(MOUSE_BUTTON_LEFT))
			{//Mouse press released
				paletteOnHold = false;
				editorValue = preview->getColor();
			}
			if (checkPaletteHover())
			{
				float horizontal(1.0f - ((inputManager->getMouseX() - palette->getX()) / palette->getWidth()));
				float vertical((inputManager->getMouseY() - palette->getY()) / palette->getHeight());
				
				Color hoverColor;
				hoverColor.r = int((float(palette->worldVertexArray[2].color.r) / 255.0f + horizontal * float(255 - palette->worldVertexArray[2].color.r) / 255.0f) * vertical * 255.0f);
				hoverColor.g = int((float(palette->worldVertexArray[2].color.g) / 255.0f + horizontal * float(255 - palette->worldVertexArray[2].color.g) / 255.0f) * vertical * 255.0f);
				hoverColor.b = int((float(palette->worldVertexArray[2].color.b) / 255.0f + horizontal * float(255 - palette->worldVertexArray[2].color.b) / 255.0f) * vertical * 255.0f);
				hoverColor.a = editorValue.a;
				preview->setColor(hoverColor);

				if (inputManager->isKeyPressed(MOUSE_BUTTON_LEFT) || (paletteOnHold && inputManager->isKeyDown(MOUSE_BUTTON_LEFT)))
					paletteOnHold = true;
			}
			else
				paletteOnHold = false;

			//Slider
			if ((inputManager->isKeyPressed(MOUSE_BUTTON_LEFT) || (sliderOnHold && inputManager->isKeyDown(MOUSE_BUTTON_LEFT))) && checkSliderHover())
			{
				sliderOnHold = true;
				sliderState = (inputManager->getMouseY() - sliderRG->getY()) / (sliderBR->getY() + sliderBR->getHeight() - sliderRG->getY());

				static const float oneSixth(1.0f / 6.0f);
				static const float fiveSixths(5.0f / 6.0f);
				static const float oneThird(1.0f / 3.0f);
				static const float twoThirds(2.0f / 3.0f);
				float r = 0.0f;
				if (sliderState < oneThird)
				{
					if (sliderState < oneSixth)
						r = 1.0f;
					else
						r = (oneThird - sliderState) / oneSixth;
				}
				else if (sliderState > twoThirds)
				{
					if (sliderState > fiveSixths)
						r = 1.0f;
					else
						r = (sliderState - twoThirds) / oneSixth;
				}
				float g = std::min(oneThird, abs(sliderState - oneThird/*green position*/));
				if (g < oneSixth)
					g = 1.0f;
				else
					g = (oneSixth - (g - oneSixth)) / oneSixth;
				float b = std::min(oneThird, abs(sliderState - twoThirds/*blue position*/));
				if (b < oneSixth)
					b = 1.0f;
				else
					b = (oneSixth - (b - oneSixth)) / oneSixth;

				palette->worldVertexArray[2].color = spehs::Color(int(r * 255.0f), int(g * 255.0f), int(b * 255.0f), 255);
			}
			else
				sliderOnHold = false;
			
			//Alpha
			if (alphaEnabled)
			{
				if (alphaOnHold && !inputManager->isKeyDown(MOUSE_BUTTON_LEFT))
				{//End alpha hold
					alphaOnHold = false;
				}

				if (checkAlphaHover())
				{
					if (inputManager->isKeyPressed(MOUSE_BUTTON_LEFT) || (alphaOnHold && inputManager->isKeyDown(MOUSE_BUTTON_LEFT)))
					{
						alphaOnHold = true;
					}

					if (alphaOnHold)
					{
						editorValue.a = int((inputManager->getMouseY() - alphaSliderBack->getY()) / alphaSliderBack->getHeight() * 255);
					}
				}
			}
		}
		else
		{
			paletteOnHold = false;
			sliderOnHold = false;
			alphaOnHold = false;
		}

		//Check changes in editor value
		ValueEditor::update();
	}
	void GUIColorEditor::visualUpdate()
	{
		GUIRectangle::visualUpdate();
		if (inputManager->mouseCollision(palette->getX(), palette->getX() + palette->getWidth(), palette->getY() + palette->getHeight(), palette->getY()) && paletteOnHold)
		{
			preview->setRenderState(getRenderState());
			preview->setPosition(inputManager->getMouseCoords());
		}
		else
			preview->setRenderState(false);

		if (alphaEnabled)
		{
			alphaSliderFront->resize(size.x / float(minSize.x) * sliderWidth, (size.y - 2 * colorEditorBorder) * (float)editorValue.a);
		}
	}
	void GUIColorEditor::onEditorValueChange()
	{
		int der(0);
		der++;
	}
	void GUIColorEditor::updateMinSize()
	{
		minSize.x = paletteWidth + sliderWidth + 3 * colorEditorBorder;
		if (alphaEnabled)
			minSize.x += colorEditorBorder + sliderWidth;
		minSize.y = paletteWidth + 2 * colorEditorBorder;
		enableState(GUIRECT_MIN_SIZE_UPDATED_BIT);
	}
	void GUIColorEditor::updateScale()
	{
		GUIRectangle::updateScale();

		const float widthForElements = alphaEnabled ? size.x - 4 * colorEditorBorder : size.x - 3 * colorEditorBorder;
		const float widthForPalette = alphaEnabled ? widthForElements * (paletteWidth / (paletteWidth + 2 * sliderWidth)) : widthForElements * (paletteWidth / (paletteWidth + sliderWidth));
		const float widthPerSlider = std::floor((widthForElements - widthForPalette) / (alphaEnabled ? 2.0f : 1.0f));
		const float elementHeight = std::floor(size.y - 2 * colorEditorBorder);
		const float sliderThirdHeight = elementHeight / 3.0f;
		palette->resize(widthForPalette, elementHeight);
		sliderRG->resize(widthPerSlider, sliderThirdHeight);
		sliderGB->resize(widthPerSlider, sliderThirdHeight);
		sliderBR->resize(widthPerSlider, sliderThirdHeight);
		if (alphaEnabled)
			alphaSliderBack->resize(widthPerSlider, elementHeight);
	}
	void GUIColorEditor::updatePosition()
	{
		GUIRectangle::updatePosition();
		float scaleX(size.x / float(minSize.x));
		palette->setPosition(getXGlobal() + colorEditorBorder, getYGlobal() + colorEditorBorder);

		int sliderThirdHeight(std::floor((size.y - 2 * colorEditorBorder) / 3.0f));
		int sliderX(palette->getX() + palette->getWidth() + colorEditorBorder);
		sliderRG->setPosition(sliderX, palette->getY());
		sliderGB->setPosition(sliderX, sliderRG->getY() + sliderRG->getHeight());
		sliderBR->setPosition(sliderX, sliderGB->getY() + sliderGB->getHeight());
		if (alphaEnabled)
		{
			alphaSliderBack->setPosition(sliderRG->getX() + sliderRG->getWidth() + colorEditorBorder, sliderRG->getY());
			alphaSliderFront->setPosition(alphaSliderBack->getX(), alphaSliderBack->getY());
		}
	}
	bool GUIColorEditor::checkPaletteHover() const
	{
		return inputManager->getMouseX() > palette->getX() && inputManager->getMouseX() < palette->getX() + palette->getWidth() &&
			inputManager->getMouseY() > palette->getY() && inputManager->getMouseY() < palette->getY() + palette->getHeight();
	}
	bool GUIColorEditor::checkSliderHover() const
	{
		return inputManager->getMouseX() > sliderRG->getX() && inputManager->getMouseX() < sliderRG->getX() + sliderRG->getWidth() &&
			inputManager->getMouseY() > sliderRG->getY() && inputManager->getMouseY() < sliderBR->getY() + sliderBR->getHeight();
	}
	bool GUIColorEditor::checkAlphaHover() const
	{
		return inputManager->getMouseX() > alphaSliderBack->getX() && inputManager->getMouseX() < alphaSliderBack->getX() + alphaSliderBack->getWidth() &&
			inputManager->getMouseY() > alphaSliderBack->getY() && inputManager->getMouseY() < alphaSliderBack->getY() + alphaSliderBack->getHeight();
	}
	void GUIColorEditor::enableAlphaEditing()
	{
		if (alphaEnabled)
			return;
		alphaEnabled = true;
		alphaSliderBack->setRenderState(getRenderState());
		alphaSliderFront->setRenderState(getRenderState());
		disableState(GUIRECT_MIN_SIZE_UPDATED_BIT);
	}
	void GUIColorEditor::disableAlphaEditing()
	{
		if (!alphaEnabled)
			return;
		editorValue.a = 255;
		alphaEnabled = false;
		alphaOnHold = false;
		alphaSliderBack->setRenderState(false);
		alphaSliderFront->setRenderState(false);
		disableState(GUIRECT_MIN_SIZE_UPDATED_BIT);
	}
	void GUIColorEditor::toggleAlphaEditing()
	{
		if (alphaEnabled)
			disableAlphaEditing();
		else
			enableAlphaEditing();
	}
}