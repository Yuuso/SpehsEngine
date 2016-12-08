#pragma once
#include "StringOperations.h"
#include "GUIRectangleRow.h"
#include "GUITextField.h"
#include "InputManager.h"
#include "Time.h"


namespace spehs
{
	/*
	For integer and floating point variables.
	*/
	template <typename Scalar>
	class GUIScalarEditor : public GUIRectangleRow
	{
	public:
		GUIScalarEditor(std::string scalarName, Scalar* _ptr) : GUIRectangleRow(),
			ptr(_ptr), floatPrecision(2), tickAmount(1), onHold(false), holdTimer(0.0f), holdTime(0.15f), initialHoldTime(1.0f),
			nameRect(new GUIRectangle(scalarName)), valueRect(new GUITextField()), decreaseRect(new GUIRectangle("-")), increaseRect(new GUIRectangle("+"))
		{
			setElementPositionMode(spehs::GUIRectangleRow::PositionMode::Right);
			updateValueTextfieldString();
			min = std::numeric_limits<Scalar>::min();
			max = std::numeric_limits<Scalar>::max();

			//Add to container
			addElement(nameRect);
			addElement(valueRect);
			addElement(decreaseRect);
			addElement(increaseRect);
		}
		~GUIScalarEditor(){}

		void setValueRange(Scalar _min, Scalar _max)
		{
			min = _min;
			max = _max;
			setValue(*ptr);
		}
		void setMinValue(Scalar _min)
		{
			min = _min;
			setValue(*ptr);
		}
		void setMaxValue(Scalar _max)
		{
			max = _max;
			setValue(*ptr);
		}
		void setTickAmount(Scalar _tickAmount){ tickAmount = _tickAmount; }
		void inputUpdate()
		{
			GUIRectangleRow::inputUpdate();

			if (valueRect->stringReady())
				setValue(getValueFromTextField());
			
			//+/- buttons
			if (inputManager->isKeyPressed(MOUSE_BUTTON_LEFT))
			{//Press
				if (increaseRect->getMouseHover())
				{//+
					onHold = true;
					holdTimer = initialHoldTime;
					setValue(*ptr + tickAmount);
				}
				else if (decreaseRect->getMouseHover())
				{//-
					onHold = true;
					holdTimer = initialHoldTime;
					setValue(*ptr - tickAmount);
				}
			}
			else if (inputManager->isKeyDown(MOUSE_BUTTON_LEFT) && onHold)
			{//Hold
				if (increaseRect->getMouseHover())
				{//+
					holdTimer -= getDeltaTime().asSeconds;
					if (holdTimer <= 0.0f)
					{
						holdTimer = holdTime;
						setValue(*ptr + tickAmount);
					}
				}
				else if (decreaseRect->getMouseHover())
				{//-
					holdTimer -= getDeltaTime().asSeconds;
					if (holdTimer <= 0.0f)
					{
						holdTimer = holdTime;
						setValue(*ptr - tickAmount);
					}
				}
				else
					onHold = false;
			}
		}

	private:
		Scalar getValueFromTextField()
		{
			std::string str(valueRect->retrieveString());
			if (std::is_integral<Scalar>::value)
				return getStringAsInt(str);
			else if (std::is_floating_point<Scalar>::value)
				return getStringAsFloat(str);
			return *ptr;
		}
		void updateValueTextfieldString()
		{
			if (std::is_integral<Scalar>::value)
				valueRect->setString(std::to_string(*ptr));
			else if (std::is_floating_point<Scalar>::value)
				valueRect->setString(spehs::toString(*ptr, floatPrecision));
			else
				valueRect->setString("# Invalid value type #");
		}
		void setValue(Scalar newValue)
		{
			if (newValue > max)
				*ptr = max;
			else if (newValue < min)
				*ptr = min;
			else
				*ptr = newValue;
			updateValueTextfieldString();
		}

		Scalar* ptr;
		Scalar min;
		Scalar max;
		Scalar tickAmount;
		int floatPrecision;
		bool onHold;
		float holdTimer;
		float holdTime;
		float initialHoldTime;

		GUIRectangle* nameRect;
		GUITextField* valueRect;
		GUIRectangle* decreaseRect;
		GUIRectangle* increaseRect;
	};
}