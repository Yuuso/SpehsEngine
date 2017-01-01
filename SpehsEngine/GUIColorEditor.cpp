#include <algorithm>
#include "GUIColorEditor.h"
#include "InputManager.h"
#include "Polygon.h"

namespace spehs
{
	float GUIColorEditor::paletteWidth = 100;
	float GUIColorEditor::sliderWidth = 10;
	int GUIColorEditor::colorEditorBorder = 3;
	float GUIColorEditor::colorPreviewWidth = 20.0f;

	GUIColorEditor::GUIColorEditor(const glm::vec4 color) : ValueEditor(color), sliderState(0.0f), sliderOnHold(false), paletteOnHold(false)
	{
		palette = Polygon::create(Shape::BUTTON, getDepth(), 1.0f, 1.0f);
		sliderRG = Polygon::create(Shape::BUTTON, getDepth(), 1.0f, 1.0f);
		sliderGB = Polygon::create(Shape::BUTTON, getDepth(), 1.0f, 1.0f);
		sliderBR = Polygon::create(Shape::BUTTON, getDepth(), 1.0f, 1.0f);
		preview = Polygon::create(6, getDepth(), colorPreviewWidth, colorPreviewWidth);
		alphaSliderBack = Polygon::create(Shape::BUTTON, getDepth(), 1.0f, 1.0f);
		alphaSliderFront = Polygon::create(Shape::BUTTON, getDepth(), 1.0f, 1.0f);

		palette->setCameraMatrixState(false);
		sliderRG->setCameraMatrixState(false);
		sliderGB->setCameraMatrixState(false);
		sliderBR->setCameraMatrixState(false);
		preview->setCameraMatrixState(false);
		alphaSliderBack->setCameraMatrixState(false);
		alphaSliderFront->setCameraMatrixState(false);
		
		//Palette colors
		palette->worldVertexArray[0].color = spehs::ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);
		palette->worldVertexArray[1].color = spehs::ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);
		palette->worldVertexArray[2].color = spehs::ColorRGBA(0.0f, 1.0f, 1.0f, 1.0f);
		palette->worldVertexArray[3].color = spehs::ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);

		//Slider colors
		//Red-Green
		sliderRG->worldVertexArray[0].color = spehs::ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f);
		sliderRG->worldVertexArray[1].color = spehs::ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f);
		sliderRG->worldVertexArray[2].color = spehs::ColorRGBA(0.0f, 1.0f, 0.0f, 1.0f);
		sliderRG->worldVertexArray[3].color = spehs::ColorRGBA(0.0f, 1.0f, 0.0f, 1.0f);
		//Green-blue
		sliderGB->worldVertexArray[0].color = spehs::ColorRGBA(0.0f, 1.0f, 0.0f, 1.0f);
		sliderGB->worldVertexArray[1].color = spehs::ColorRGBA(0.0f, 1.0f, 0.0f, 1.0f);
		sliderGB->worldVertexArray[2].color = spehs::ColorRGBA(0.0f, 0.0f, 1.0f, 1.0f);
		sliderGB->worldVertexArray[3].color = spehs::ColorRGBA(0.0f, 0.0f, 1.0f, 1.0f);
		//Blue-red
		sliderBR->worldVertexArray[0].color = spehs::ColorRGBA(0.0f, 0.0f, 1.0f, 1.0f);
		sliderBR->worldVertexArray[1].color = spehs::ColorRGBA(0.0f, 0.0f, 1.0f, 1.0f);
		sliderBR->worldVertexArray[2].color = spehs::ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f);
		sliderBR->worldVertexArray[3].color = spehs::ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f);

		//Alpha slider colors
		alphaSliderBack->setColor((unsigned char)0, 0, 0, 255);
		alphaSliderFront->setColor((unsigned char)255, 255, 255, 255);

		setDepth(getDepth());
		setRenderState(getRenderState());
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
	void GUIColorEditor::inputUpdate()
	{
		GUIRectangle::inputUpdate();
		
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
				
				glm::vec4 hoverColor;
				hoverColor.r = (palette->worldVertexArray[2].color.rgba.r + horizontal * (1.0f - palette->worldVertexArray[2].color.rgba.r)) * vertical;
				hoverColor.g = (palette->worldVertexArray[2].color.rgba.g + horizontal * (1.0f - palette->worldVertexArray[2].color.rgba.g)) * vertical;
				hoverColor.b = (palette->worldVertexArray[2].color.rgba.b + horizontal * (1.0f - palette->worldVertexArray[2].color.rgba.b)) * vertical;
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

				palette->worldVertexArray[2].color = spehs::ColorRGBA(r, g, b, 1.0f);
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
						editorValue.a = (inputManager->getMouseY() - alphaSliderBack->getY()) / alphaSliderBack->getHeight();
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
		if (getMouseHoverAny() && paletteOnHold)
		{
			preview->setRenderState(getRenderState());
			preview->setPosition(inputManager->getMouseCoords());
		}
		else
			preview->setRenderState(false);

		if (alphaEnabled)
		{
			alphaSliderFront->resize(size.x / float(minSize.x) * sliderWidth, (size.y - 2 * colorEditorBorder) * editorValue.a);
		}
	}
	void GUIColorEditor::onEditorValueChange()
	{
		int der(0);
		der++;
	}
	void GUIColorEditor::updateMinSize()
	{
		minSize.x = paletteWidth + sliderWidth + 2 * colorEditorBorder;
		if (alphaEnabled)
			minSize.x += colorEditorBorder + sliderWidth;
		minSize.y = paletteWidth + 2 * colorEditorBorder;
		enableState(GUIRECT_MIN_SIZE_UPDATED_BIT);
	}
	void GUIColorEditor::updateScale()
	{
		GUIRectangle::updateScale();

		float scaleX(size.x / float(minSize.x));

		int sizeX(size.x - colorEditorBorder * (alphaEnabled ? 3 : 2));
		palette->resize(std::floor(scaleX * paletteWidth), size.y - 2 * colorEditorBorder);
		float sliderThirdHeight(std::floor(size.y / 3.0f));
		sliderRG->resize(std::floor(scaleX * sliderWidth), sliderThirdHeight);
		sliderGB->resize(std::floor(scaleX * sliderWidth), sliderThirdHeight);
		sliderBR->resize(std::floor(scaleX * sliderWidth), sliderThirdHeight);
		if (alphaEnabled)
			alphaSliderBack->resize(std::floor(scaleX * sliderWidth), size.y - 2 * colorEditorBorder);
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
		editorValue.a = 1.0f;
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