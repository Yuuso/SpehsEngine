
#pragma once

#include <cstring>


//C Array with some helper functions


namespace spehs
{
	template<typename type>
	class Container
	{
		template<typename type> friend Container<type>& operator+(const Container<type>& _first, const Container<type>& _second);
	public:
		Container();
		Container(size_t _size);
		Container(type* _data, size_t _dataSize);
		~Container();

		Container<type>& operator+=(const Container<type>& _other);
		Container<type>& operator=(const Container<type>& _other);
		type& operator[](const size_t& _value) const;


		void push(const type& _value);
		void push(const Container<type>& _values);
		void resize(size_t _size);
		void grow(size_t _size);
		void clear();

		size_t size() const;
		type& back() const;
		void* data() const;
		void* data(size_t _position) const;

	private:
		type* dataArray;
		size_t dataSize;
	};


	//Methods:
	template<typename type>
	Container<type>::Container()
	{
		dataArray = nullptr;
		dataSize = 0;
	}
	template<typename type>
	Container<type>::Container(size_t _size)
	{
		dataArray = new type[_size];
		dataSize = _size;
	}
	template<typename type>
	Container<type>::Container(type* _data, size_t _dataSize)
	{
		dataArray = new type[_dataSize];
		memcpy((void*) &dataArray[0], (void*) &_data[0], _dataSize * sizeof(type));
		dataSize = _dataSize;
	}
	template<typename type>
	Container<type>::~Container()
	{
		if (dataArray)
			delete[] dataArray;
	}

	template<typename type>
	Container<type>& operator+(const Container<type>& _first, const Container<type>& _second)
	{
		Container<type>* result = new Container<type>(_first.dataSize + _second.dataSize);
		memcpy(result->data(), _first.data(), _first.dataSize * sizeof(type));
		memcpy(result->data(_first.dataSize), _second.data(), _second.dataSize * sizeof(type));
		return *result;
	}


	template<typename type>
	Container<type>& Container<type>::operator+=(const Container<type>& _other)
	{
		this->grow(_other.dataSize);
		memcpy(this->data(dataSize - _other.dataSize), _other.data(), _other.dataSize * sizeof(type));
		return *this;
	}
	template<typename type>
	Container<type>& Container<type>::operator=(const Container<type>& _other)
	{
		this->resize(_other.dataSize);
		memcpy(this->data(), _other.data(), dataSize * sizeof(type));
		return *this;
	}
	template<typename type>
	type& Container<type>::operator[](const size_t& _value) const
	{
		return dataArray[_value];
	}


	template<typename type>
	void Container<type>::push(const type& _value)
	{
		grow(1);
		dataArray[dataSize - 1] = _value;
	}
	template<typename type>
	void Container<type>::push(const Container<type>& _values)
	{
		*this += _values;
	}
	template<typename type>
	void Container<type>::resize(size_t _size)
	{
		type* newArray = new type[_size];
		if (dataSize > _size)
			dataSize = _size;
		memcpy((void*) &newArray[0], (void*) &dataArray[0], dataSize * sizeof(type));
		if (dataSize < _size)
			dataSize = _size;
		if (dataArray)
			delete [] dataArray;
		dataArray = newArray;
	}
	template<typename type>
	void Container<type>::grow(size_t _size)
	{
		type* newArray = new type[dataSize + _size];
		memcpy((void*) &newArray[0], (void*) &dataArray[0], dataSize * sizeof(type));
		dataSize += _size;
		if (dataArray)
			delete [] dataArray;
		dataArray = newArray;
	}
	template<typename type>
	void Container<type>::clear()
	{
		if (dataArray)
		{
			delete [] dataArray;
			dataArray = nullptr;
		}
		dataSize = 0;
	}
	template<typename type>
	size_t Container<type>::size() const
	{
		return dataSize;
	}
	template<typename type>
	type& Container<type>::back() const
	{
		return dataArray[dataSize - 1];
	}
	template<typename type>
	void* Container<type>::data() const
	{
		return (void*) &dataArray[0];
	}
	template<typename type>
	void* Container<type>::data(size_t _position) const
	{
		return (void*) &dataArray[_position];
	}
}