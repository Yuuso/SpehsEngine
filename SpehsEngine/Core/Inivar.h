#pragma once


namespace se
{
	class Inisection;
	class Inifile;
	class AbstractInivar
	{
	public:

		const std::string name;
		const std::string type;

	protected:
		friend class Inifile;
		friend class Inisection;
		AbstractInivar(Inisection& _section, const std::string& name, const std::string& type);
		virtual ~AbstractInivar();
		void markForUnwrittenChanges();
		bool tryRetrieveValue();

		/* Called when the inivar is written to the human readable format file. */
		virtual std::string toString() const = 0;
		/* Called when the inivar is read from the human readable format file. */
		virtual bool fromString(const std::string& string) = 0;
	private:
		Inisection& section;
	};

	template<typename T>
	class Inivar : public AbstractInivar
	{
	public:
		T operator=(const T newValue);
		operator T() const;
		bool operator==(const T &other) const;
		bool operator!=(const T &other) const;

		std::string toString() const override;
		bool fromString(const std::string& string) override;

		boost::signals2::signal<void(const Inivar<T>&)> changedSignal;
	private:
		friend class Inisection;
		Inivar(Inisection& _section, const std::string& _name, const T& _value);
		Inivar(const Inivar<T>& other) = delete;
		Inivar(const Inivar<T>&& other) = delete;
		void operator=(const Inivar<T>& other) = delete;
		void operator=(const Inivar<T>&& other) = delete;
		mutable std::recursive_mutex mutex;
		T value;
	};

	// Get nicely formated type name
	template<typename T>
	inline std::string getTypeName()
	{
		return typeid(T).name();
	}
	template<>
	inline std::string getTypeName<std::string>()
	{
		return "std::string";
	}


	// TEMPLATE DEFINITIONS

	template<typename T>
	Inivar<T>::Inivar(Inisection& _section, const std::string& _name, const T& _value)
		: AbstractInivar(_section, _name, getTypeName<T>())
		, value(_value)
	{
	}

	template<typename T>
	T Inivar<T>::operator=(const T newValue)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (value != newValue)
		{
			value = newValue;
			markForUnwrittenChanges();
			changedSignal(*this);
		}
		return value;
	}

	template<typename T>
	Inivar<T>::operator T() const
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return value;
	}

	template<typename T>
	bool Inivar<T>::operator==(const T &other) const
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return value == other;
	}

	template<typename T>
	bool Inivar<T>::operator!=(const T &other) const
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return value != other;

	}

	template<typename T>
	bool operator==(const T &other, Inivar<T> &inivar)
	{
		return inivar.operator==(other);
	}

	template<typename T>
	bool operator!=(const T &other, Inivar<T> &inivar)
	{
		return inivar.operator!=(other);
	}

	//template<typename T>
	//T& operator=(const T &other, Inivar<T> &inivar)
	//{
	//	return other = (T)inivar;
	//}
}