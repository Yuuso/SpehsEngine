#pragma once


namespace se
{
	/*
		Stored value signals when changed using the setter.
	*/
	template<typename T>
	class WatchedValue
	{
	public:
		WatchedValue() = default;
		WatchedValue(const T& v) : value(v) {}
		
		void set(const T& _value)
		{
			T oldValue;
			std::swap(value, oldValue);
			value = _value;
			changedSignal(oldValue, value);
			if (changedCallback)
			{
				changedCallback(oldValue, value);
			}
		}

		operator const T&() const
		{
			return value;
		}

		const T& get() const
		{
			return value;
		}

		void connectToChangedSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(const T&, const T&)>& callback)
		{
			scopedConnection = changedSignal.connect(callback);
		}

		void setValueChangedCallback(const std::function<void(const T&, const T&)>& callback)
		{
			if (changedCallback)
			{
				se_assert(!callback && "Callback has already been set. Setting the callback erases the previously set callback!");
			}
			changedCallback = callback;
		}

	private:
		T value;
		boost::signals2::signal<void(const T&, const T&)> changedSignal;
		std::function<void(const T&, const T&)> changedCallback;
	};
}