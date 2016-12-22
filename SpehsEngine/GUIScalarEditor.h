#pragma once
#include "StringOperations.h"
#include "GUIRectangleRow.h"
#include "GUIStringEditor.h"
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
		static_assert(std::is_arithmetic<Scalar>::value, "Scalar editor type must be of either integral or floating type!");
		enum class EditorType { Slider, Ticks};
	public:
		GUIScalarEditor(const std::string scalarName, Scalar& scalarReference) :
			scalar(scalarReference), floatPrecision(2), tickAmount(1), onHold(false), holdTimer(0.0f), holdTime(0.15f), initialHoldTime(1.0f),
			nameRect(new GUIRectangle(scalarName)), valueRect(new GUIStringEditor()), decreaseRect(new GUIRectangle("-")), increaseRect(new GUIRectangle("+"))
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

		void setValueRange(const Scalar _min, const Scalar _max)
		{
			min = _min;
			max = _max;
			setValue(scalar);
		}
		void setMinValue(const Scalar _min)
		{
			min = _min;
			setValue(scalar);
		}
		void setMaxValue(const Scalar _max)
		{
			max = _max;
			setValue(scalar);
		}
		void setTickAmount(const Scalar _tickAmount){ tickAmount = _tickAmount; }
		void inputUpdate() override
		{
			GUIRectangleRow::inputUpdate();

			if (valueRect->valueEdited())
				setValue(getValueFromTextField());
			
			//+/- buttons
			if (inputManager->isKeyPressed(MOUSE_BUTTON_LEFT))
			{//Press
				if (increaseRect->getMouseHover())
				{//+
					onHold = true;
					holdTimer = initialHoldTime;
					setValue(scalar + tickAmount);
				}
				else if (decreaseRect->getMouseHover())
				{//-
					onHold = true;
					holdTimer = initialHoldTime;
					setValue(scalar - tickAmount);
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
						setValue(scalar + tickAmount);
					}
				}
				else if (decreaseRect->getMouseHover())
				{//-
					holdTimer -= time::getDeltaTimeAsSeconds();
					if (holdTimer <= 0.0f)
					{
						holdTimer = holdTime;
						setValue(scalar - tickAmount);
					}
				}
				else
					onHold = false;
			}
		}

	private:
		Scalar getValueFromTextField() const
		{
			std::string str(valueRect->retrieveString());
			if (std::is_integral<Scalar>::value)
				return getStringAsInt(str);
			else if (std::is_floating_point<Scalar>::value)
				return getStringAsFloat(str);
			return scalar;
		}
		void updateValueTextfieldString()
		{
			if (std::is_integral<Scalar>::value)
				valueRect->setString(std::to_string(scalar));
			else if (std::is_floating_point<Scalar>::value)
				valueRect->setString(spehs::toString(scalar, floatPrecision));
			else
				valueRect->setString("# Invalid value type #");
		}
		void setValue(const Scalar newValue)
		{
			if (newValue > max)
				scalar = max;
			else if (newValue < min)
				scalar = min;
			else
				scalar = newValue;
			updateValueTextfieldString();
		}

		Scalar& scalar;
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