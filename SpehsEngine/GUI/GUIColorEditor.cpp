#include <algorithm>
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/GUI/GUIColorEditor.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/Polygon.h"

namespace spehs
{
	float GUIColorEditor::paletteWidth = 100;
	float GUIColorEditor::sliderWidth = 10;
	int GUIColorEditor::colorEditorBorder = 3;
	float GUIColorEditor::colorPreviewWidth = 20.0f;

	GUIColorEditor::GUIColorEditor(GUIContext& context)
		: GUIRectangle(context)
		, ValueEditor(Color(1.0f, 1.0f, 1.0f, 1.0f))
		, sliderState(0.0f)
		, sliderOnHold(false)
		, paletteOnHold(false)
	{
		palette = getBatchManager().createPolygon(Shape::BUTTON, getDepth(), 1.0f, 1.0f);
		sliderRG = getBatchManager().createPolygon(Shape::BUTTON, getDepth(), 1.0f, 1.0f);
		sliderGB = getBatchManager().createPolygon(Shape::BUTTON, getDepth(), 1.0f, 1.0f);
		sliderBR = getBatchManager().createPolygon(Shape::BUTTON, getDepth(), 1.0f, 1.0f);
		preview = getBatchManager().createPolygon(6, getDepth(), colorPreviewWidth, colorPreviewWidth);
		alphaSliderBack = getBatchManager().createPolygon(Shape::BUTTON, getDepth(), 1.0f, 1.0f);
		alphaSliderFront = getBatchManager().createPolygon(Shape::BUTTON, getDepth(), 1.0f, 1.0f);

		palette->setCameraMatrixState(false);
		sliderRG->setCameraMatrixState(false);
		sliderGB->setCameraMatrixState(false);
		sliderBR->setCameraMatrixState(false);
		preview->setCameraMatrixState(false);
		alphaSliderBack->setCameraMatrixState(false);
		alphaSliderFront->setCameraMatrixState(false);

		//Palette colors
		palette->worldVertexArray[0].color = spehs::Color(0.0f, 0.0f, 0.0f, 1.0f);
		palette->worldVertexArray[1].color = spehs::Color(0.0f, 0.0f, 0.0f, 1.0f);
		palette->worldVertexArray[2].color = spehs::Color(0.0f, 1.0f, 1.0f, 1.0f);
		palette->worldVertexArray[3].color = spehs::Color(1.0f, 1.0f, 1.0f, 1.0f);

		//Slider colors
		//Red-Green
		sliderRG->worldVertexArray[0].color = spehs::Color(1.0f, 0.0f, 0.0f, 1.0f);
		sliderRG->worldVertexArray[1].color = spehs::Color(1.0f, 0.0f, 0.0f, 1.0f);
		sliderRG->worldVertexArray[2].color = spehs::Color(0.0f, 1.0f, 0.0f, 1.0f);
		sliderRG->worldVertexArray[3].color = spehs::Color(0.0f, 1.0f, 0.0f, 1.0f);
		//Green-blue
		sliderGB->worldVertexArray[0].color = spehs::Color(0.0f, 1.0f, 0.0f, 1.0f);
		sliderGB->worldVertexArray[1].color = spehs::Color(0.0f, 1.0f, 0.0f, 1.0f);
		sliderGB->worldVertexArray[2].color = spehs::Color(0.0f, 0.0f, 1.0f, 1.0f);
		sliderGB->worldVertexArray[3].color = spehs::Color(0.0f, 0.0f, 1.0f, 1.0f);
		//Blue-red
		sliderBR->worldVertexArray[0].color = spehs::Color(0.0f, 0.0f, 1.0f, 1.0f);
		sliderBR->worldVertexArray[1].color = spehs::Color(0.0f, 0.0f, 1.0f, 1.0f);
		sliderBR->worldVertexArray[2].color = spehs::Color(1.0f, 0.0f, 0.0f, 1.0f);
		sliderBR->worldVertexArray[3].color = spehs::Color(1.0f, 0.0f, 0.0f, 1.0f);

		//Alpha slider colors
		alphaSliderBack->setColor(spehs::Color(0.0f, 0.0f, 0.0f, 1.0f));
		alphaSliderFront->setColor(spehs::Color(1.0f, 1.0f, 1.0f, 1.0f));

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

	void GUIColorEditor::inputUpdate()
	{
		GUIRectangle::inputUpdate();

		if (getInputEnabled())
		{
			//Palette
			if (paletteOnHold && !getInputManager().isKeyDown(MOUSE_BUTTON_LEFT))
			{//Mouse press released
				paletteOnHold = false;
				editorValue = preview->getColor();
			}
			if (checkPaletteHover())
			{
				float horizontal(1.0f - ((getInputManager().getMouseX() - palette->getX()) / palette->getWidth()));
				float vertical((getInputManager().getMouseY() - palette->getY()) / palette->getHeight());

				Color hoverColor;
				hoverColor.r = palette->worldVertexArray[2].color.r + horizontal * (1.0f - palette->worldVertexArray[2].color.r) * vertical;
				hoverColor.g = palette->worldVertexArray[2].color.g + horizontal * (1.0f - palette->worldVertexArray[2].color.g) * vertical;
				hoverColor.b = palette->worldVertexArray[2].color.b + horizontal * (1.0f - palette->worldVertexArray[2].color.b) * vertical;
				hoverColor.a = editorValue.a;
				preview->setColor(hoverColor);

				if (getInputManager().isKeyPressed(MOUSE_BUTTON_LEFT) || (paletteOnHold && getInputManager().isKeyDown(MOUSE_BUTTON_LEFT)))
					paletteOnHold = true;
			}
			else
				paletteOnHold = false;

			//Slider
			if ((getInputManager().isKeyPressed(MOUSE_BUTTON_LEFT) || (sliderOnHold && getInputManager().isKeyDown(MOUSE_BUTTON_LEFT))) && checkSliderHover())
			{
				sliderOnHold = true;
				sliderState = (getInputManager().getMouseY() - sliderRG->getY()) / (sliderBR->getY() + sliderBR->getHeight() - sliderRG->getY());

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

				palette->worldVertexArray[2].color = spehs::Color(r, g, b, 1.0f);
			}
			else
				sliderOnHold = false;

			//Alpha
			if (alphaEnabled)
			{
				if (alphaOnHold && !getInputManager().isKeyDown(MOUSE_BUTTON_LEFT))
				{//End alpha hold
					alphaOnHold = false;
				}

				if (checkAlphaHover())
				{
					if (getInputManager().isKeyPressed(MOUSE_BUTTON_LEFT) || (alphaOnHold && getInputManager().isKeyDown(MOUSE_BUTTON_LEFT)))
					{
						alphaOnHold = true;
					}

					if (alphaOnHold)
					{
						editorValue.a = (getInputManager().getMouseY() - alphaSliderBack->getY()) / alphaSliderBack->getHeight();
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
		if (getInputManager().mouseCollision(palette->getX(), palette->getX() + palette->getWidth(), palette->getY() + palette->getHeight(), palette->getY()) && paletteOnHold)
		{
			preview->setRenderState(getRenderState());
			preview->setPosition(getInputManager().getMouseCoords());
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
		minSize.x = int(paletteWidth + sliderWidth + 3.0f * (float)colorEditorBorder);
		if (alphaEnabled)
			minSize.x += colorEditorBorder + (int)sliderWidth;
		minSize.y = int(paletteWidth + 2.0f * (float)colorEditorBorder);
		enableState(GUIRECT_MIN_SIZE_UPDATED_BIT);
	}

	void GUIColorEditor::updateScale()
	{
		GUIRectangle::updateScale();

		const float widthForElements = alphaEnabled ? float(size.x - 4 * colorEditorBorder) : float(size.x - 3 * colorEditorBorder);
		const float widthForPalette = alphaEnabled ? widthForElements * (paletteWidth / (paletteWidth + 2.0f * sliderWidth)) : widthForElements * (paletteWidth / (paletteWidth + sliderWidth));
		const float widthPerSlider = std::floor((widthForElements - widthForPalette) / (alphaEnabled ? 2.0f : 1.0f));
		const float elementHeight = float(size.y - 2 * colorEditorBorder);
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
		palette->setPosition(float(getXGlobal() + colorEditorBorder), float(getYGlobal() + colorEditorBorder));

		const int sliderThirdHeight = int(std::floor(float(size.y - 2 * colorEditorBorder) / 3.0f));
		const int sliderX = int(palette->getX() + palette->getWidth()) + colorEditorBorder;
		sliderRG->setPosition(float(sliderX), palette->getY());
		sliderGB->setPosition(float(sliderX), sliderRG->getY() + sliderRG->getHeight());
		sliderBR->setPosition(float(sliderX), sliderGB->getY() + sliderGB->getHeight());
		if (alphaEnabled)
		{
			alphaSliderBack->setPosition(sliderRG->getX() + sliderRG->getWidth() + colorEditorBorder, sliderRG->getY());
			alphaSliderFront->setPosition(alphaSliderBack->getX(), alphaSliderBack->getY());
		}
	}

	bool GUIColorEditor::checkPaletteHover() const
	{
		return getInputManager().getMouseX() > palette->getX() && getInputManager().getMouseX() < palette->getX() + palette->getWidth() &&
			getInputManager().getMouseY() > palette->getY() && getInputManager().getMouseY() < palette->getY() + palette->getHeight();
	}

	bool GUIColorEditor::checkSliderHover() const
	{
		return getInputManager().getMouseX() > sliderRG->getX() && getInputManager().getMouseX() < sliderRG->getX() + sliderRG->getWidth() &&
			getInputManager().getMouseY() > sliderRG->getY() && getInputManager().getMouseY() < sliderBR->getY() + sliderBR->getHeight();
	}

	bool GUIColorEditor::checkAlphaHover() const
	{
		return getInputManager().getMouseX() > alphaSliderBack->getX() && getInputManager().getMouseX() < alphaSliderBack->getX() + alphaSliderBack->getWidth() &&
			getInputManager().getMouseY() > alphaSliderBack->getY() && getInputManager().getMouseY() < alphaSliderBack->getY() + alphaSliderBack->getHeight();
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