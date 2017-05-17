#pragma once
#include "StringOperations.h"
#include "GUIRectangleRow.h"
#include "GUIStringEditor.h"
#include "InputManager.h"
#include "ValueEditor.h"
#include "Time.h"


namespace spehs
{
	/*
	For integer and floating point variables.
	*/
	template <typename Scalar>
	class GUIScalarEditor : public GUIRectangleRow, public ValueEditor<Scalar>
	{
	public:
		static_assert(std::is_arithmetic<Scalar>::value, "Scalar editor type must be of either integral or floating type!");
		enum class EditorType { Slider, Ticks };
	public:
		GUIScalarEditor(const std::string& scalarName, Scalar scalarEditorValue) : ValueEditor(scalarEditorValue),
			floatPrecision(2), tickAmount(1), onHold(false), holdTimer(0.0f), holdTime(0.15f), initialHoldTime(1.0f),
			nameRect(new GUIRectangle(scalarName)), valueRect(new GUIStringEditor()), decreaseRect(new GUIRectangle("-")), increaseRect(new GUIRectangle("+"))
		{
			setElementPositionMode(spehs::GUIRectangleRow::PositionMode::StackRight);
			min = std::numeric_limits<Scalar>::min();
			max = std::numeric_limits<Scalar>::max();

			//Add to container
			addElement(nameRect);
			addElement(valueRect);
			addElement(decreaseRect);
			addElement(increaseRect);
		}
		~GUIScalarEditor(){}

		void setString(const std::string& scalarName) override
		{
			nameRect->setString(scalarName);
		}
		void setValueRange(const Scalar _min, const Scalar _max)
		{
			min = _min;
			max = _max;
			setEditorValue(getEditorValue());
		}
		void setMinValue(const Scalar _min)
		{
			min = _min;
			setEditorValue(getEditorValue());
		}
		void setMaxValue(const Scalar _max)
		{
			max = _max;
			setEditorValue(getEditorValue());
		}
		void setTickAmount(const Scalar _tickAmount){ tickAmount = _tickAmount; }
		void inputUpdate() override
		{
			ValueEditor::update();
			GUIRectangleRow::inputUpdate();

			if (valueRect->editorValueChanged())
				setEditorValue(getValueFromTextField());
			
			//+/- buttons
			if (inputManager->isKeyPressed(MOUSE_BUTTON_LEFT))
			{//Press
				if (increaseRect->getMouseHover())
				{//+
					onHold = true;
					holdTimer = initialHoldTime;
					setEditorValue(editorValue + tickAmount);
				}
				else if (decreaseRect->getMouseHover())
				{//-
					onHold = true;
					holdTimer = initialHoldTime;
					setEditorValue(editorValue - tickAmount);
				}
			}
			else if (inputManager->isKeyDown(MOUSE_BUTTON_LEFT) && onHold)
			{//Hold
				if (increaseRect->getMouseHover())
				{//+
					holdTimer -= time::getDeltaTimeAsSeconds();
					if (holdTimer <= 0.0f)
					{
						holdTimer = holdTime;
						setEditorValue(editorValue + tickAmount);
					}
				}
				else if (decreaseRect->getMouseHover())
				{//-
					holdTimer -= time::getDeltaTimeAsSeconds();
					if (holdTimer <= 0.0f)
					{
						holdTimer = holdTime;
						setEditorValue(editorValue - tickAmount);
					}
				}
				else
					onHold = false;
			}
		}
		void setEditorValue(const Scalar newValue) override
		{
			if (newValue > max)
				ValueEditor::setEditorValue(max);
			else if (newValue < min)
				ValueEditor::setEditorValue(min);
			else
				ValueEditor::setEditorValue(newValue);
		}
		void setColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a = 255)
		{
			GUIRectangle::setColor(r, g, b, a);
			for (unsigned i = 0; i < elements.size(); i++)
				elements[i]->setColor(r, g, b, a);
		}
		void setColor(const float r, const float g, const float b, const float a = 1.0f)
		{
			GUIRectangle::setColor(r, g, b, a);
			for (unsigned i = 0; i < elements.size(); i++)
				elements[i]->setColor(r, g, b, a);
		}
		void setColor(const glm::vec3& color)
		{
			GUIRectangle::setColor(color);
			for (unsigned i = 0; i < elements.size(); i++)
				elements[i]->setColor(color);
		}
		void setColor(const glm::vec4& color)
		{
			GUIRectangle::setColor(color);
			for (unsigned i = 0; i < elements.size(); i++)
				elements[i]->setColor(color);
		}
		void addElement(spehs::GUIRectangle* element) override
		{
			GUIRectangleRow::addElement(element);
			element->setColor(getColor());
		}

	protected:
		void onEditorValueChange() override
		{
			if (std::is_integral<Scalar>::value)
				valueRect->setString(std::to_string(getEditorValue()));
			else if (std::is_floating_point<Scalar>::value)
				valueRect->setString(spehs::toString(getEditorValue(), floatPrecision));
			else
				valueRect->setString("# Invalid value type #");
		}

	private:
		Scalar getValueFromTextField() const
		{
			const std::string str(valueRect->getEditorValue());
			if (std::is_integral<Scalar>::value)
				return getStringAsInt(str);
			else if (std::is_floating_point<Scalar>::value)
				return getStringAsFloat(str);
			else
				return 0;
		}

		Scalar min;
		Scalar max;
		Scalar tickAmount;
		int floatPrecision;
		bool onHold;
		float holdTimer;
		float holdTime;
		float initialHoldTime;

		GUIRectangle* nameRect;
		GUIStringEditor* valueRect;
		GUIRectangle* decreaseRect;
		GUIRectangle* increaseRect;
	};
}