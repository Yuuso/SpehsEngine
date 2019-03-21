#pragma once
#include "boost/signals2.hpp"

namespace se
{
	/**Base class/interface for a "value editor".
	Important: ValueType must have the == and = operators implemented: After the = operator, the result of == operator should return true!
	*/
	template <typename ValueType>
	class ValueEditor
	{
	public:
		ValueEditor(const ValueType& initialValue) : value(initialValue)
		{
		}

		void valueEditorUpdate()
		{
			if (valueChanged)
			{
				valueChanged = false;
				onEditorValueChange();
				if (valueChangedCallback)
				{
					valueChangedCallback(value);
				}
				valueChangedSignal(value);
			}
		}

		/**Returns true when user has edited the value and the value is ready to be processed. Should return true during a full program loop cycle (from input update to the next)*/
		bool editorValueChanged() const
		{
			return valueChanged;
		}

		const ValueType& getEditorValue() const
		{
			return value;
		}

		/* Value can be set by anyone. */
		virtual void setEditorValue(const ValueType& newValue)
		{
			if (value != newValue)
			{
				valueChanged = true;
				value = newValue;
			}
		}

		void setValueChangedCallback(const std::function<void(const ValueType&)>& callback) { valueChangedCallback = callback; }

		void connectToValueChangedSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<void(const ValueType&)>& callback) { scopedConnection = valueChangedSignal.connect(callback); }
		
	protected:

		/**The GUI editor must implement this method in order to relay the changed editor value to the user through the graphical interface.*/
		virtual void onEditorValueChange() = 0;

	private:

		/**The current value of the editors value type*/
		ValueType value;
		bool valueChanged = false;
		std::function<void(const ValueType&)> valueChangedCallback;
		boost::signals2::signal<void(const ValueType&)> valueChangedSignal;
	};
}