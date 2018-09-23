#pragma once
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/GUI/GUIRectangleRow.h"
#include "SpehsEngine/GUI/GUIStringEditor.h"
#include "SpehsEngine/GUI/ValueEditor.h"


namespace se
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
		GUIScalarEditor(GUIContext& context, const std::string& scalarName, Scalar scalarEditorValue)
			: GUIRectangleRow(context)
			, ValueEditor<Scalar>(scalarEditorValue)
			, floatPrecision(2)
			, tickAmount(1)
			, onHold(false)
			, holdTimer(0)
			, holdTime(time::fromSeconds(0.15f))
			, initialHoldTime(time::fromSeconds(1.0f))
			, nameRect(new GUIRectangle(context))
			, valueRect(new GUIStringEditor(context))
			, decreaseRect(new GUIRectangle(context))
			, increaseRect(new GUIRectangle(context))
		{
			nameRect->setString(scalarName);
			decreaseRect->setString("-");
			increaseRect->setString("+");
			setElementPositionMode(se::GUIRectangleRow::PositionMode::StackRight);
			min = std::numeric_limits<Scalar>::min();
			max = std::numeric_limits<Scalar>::max();

			//Add to container
			addElement(nameRect);
			addElement(valueRect);
			addElement(decreaseRect);
			addElement(increaseRect);

			nameRect->disableState(GUIRECT_HOVER_COLOR_BIT);
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
			if (getInputManager().isKeyPressed(MOUSE_BUTTON_LEFT))
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
			else if (getInputManager().isKeyDown(MOUSE_BUTTON_LEFT) && onHold)
			{//Hold
				if (increaseRect->getMouseHover())
				{//+
					holdTimer -= getDeltaTime();
					if (holdTimer <= se::time::zero)
					{
						holdTimer = holdTime;
						setEditorValue(editorValue + tickAmount);
					}
				}
				else if (decreaseRect->getMouseHover())
				{//-
					holdTimer -= getDeltaTime();
					if (holdTimer <= se::time::zero)
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
		void setColor(const se::Color& color)
		{
			GUIRectangle::setColor(color);
			for (unsigned i = 0; i < elements.size(); i++)
				elements[i]->setColor(color);
		}
		void setAlpha(const float alpha)
		{
			GUIRectangle::setAlpha(alpha);
			for (unsigned i = 0; i < elements.size(); i++)
				elements[i]->setAlpha(alpha);
		}
		void addElement(se::GUIRectangle* element) override
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
				valueRect->setString(se::toString(float(getEditorValue()), floatPrecision));
			else
				valueRect->setString("# Invalid value type #");
		}

	private:
		Scalar getValueFromTextField() const
		{
			const std::string str(valueRect->getEditorValue());
			if (std::is_integral<Scalar>::value)
				return Scalar(getStringAsInt(str));
			else if (std::is_floating_point<Scalar>::value)
				return Scalar(getStringAsFloat(str));
			else
				return 0;
		}

		Scalar min;
		Scalar max;
		Scalar tickAmount;
		int floatPrecision;
		bool onHold;
		time::Time holdTimer;
		time::Time holdTime;
		time::Time initialHoldTime;

		GUIRectangle* nameRect;
		GUIStringEditor* valueRect;
		GUIRectangle* decreaseRect;
		GUIRectangle* increaseRect;
	};
}