#pragma once

namespace se
{
	/**Base class/interface for a "value editor".
	Important: ValueType must have the == and = operators implemented: After the = operator, the result of == operator should return true!
	*/
	template <typename ValueType>
	class ValueEditor
	{
	public:
		ValueEditor(ValueType initialValue) : _initialized(false), _valueChanged(false), editorValue(initialValue), previousEditorValue(initialValue)
		{
		}

		/**Input update will detect value change. Deriving class is responsible for calling the update method! */
		virtual void update()
		{
			if (!_initialized)
			{
				_initialized = true;
				previousEditorValue = editorValue;
				onEditorValueChange();
			}

			if (editorValue == previousEditorValue)
				_valueChanged = false;
			else
			{//Value change detected
				onEditorValueChange();//Implementation reacts visually to the change
				_valueChanged = true;
				previousEditorValue = editorValue;
			}
		}

		/**Returns true when user has edited the value and the value is ready to be processed. Should return true during a full program loop cycle (from input update to the next)*/
		bool editorValueChanged() const
		{
			return _valueChanged/*Update has been run, value change was automatically detected*/ ||
				!(editorValue == previousEditorValue)/*Update hasn't run yet, but change can be detected using previous value data*/;
		}

		/**Retrieves the editor's value*/
		ValueType getEditorValue() const
		{
			return editorValue;
		}

		/**Outside source sets the editor's value*/
		virtual void setEditorValue(const ValueType newValue)
		{
			if (editorValue != newValue)
			{
				editorValue = newValue;
				onEditorValueChange();
			}
		}

	protected:

		/**The GUI editor must implement this method in order to relay the changed editor value to the user through the graphical interface.*/
		virtual void onEditorValueChange() = 0;

		/**The current value of the editors value type*/
		ValueType editorValue;

	private:
		bool _initialized;
		bool _valueChanged;
		ValueType previousEditorValue;//Used to automatically detect a change in the editor value between the loops (This is donw using the != operator)
	};
}