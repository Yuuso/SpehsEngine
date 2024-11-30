#pragma once


namespace se
{
	struct SerialTagVector {};
	template<typename T> struct SerialTag<std::vector<T>> { typedef SerialTagVector type; };
	template<> template<typename S, typename T>
	static bool Serial<SerialTagVector>::serial(S& _serial, T _vector)
	{
		typedef typename remove_cvref<T>::type VectorType;
		typedef typename VectorType::value_type ValueType;
		if constexpr (S::getWritingEnabled())
		{
			const uint32_t size = uint32_t(_vector.size());
			se_serial(_serial, size, "s");
			size_t counter = 0;
			(void)counter;
			for (const ValueType& value : _vector)
			{
				se_serial(_serial, value, std::to_string(counter++));
			}
			return true;
		}
		else
		{
			VectorType temp;
			uint32_t size = 0;
			se_serial(_serial, size, "s");
			for (uint32_t i = 0; i < size; i++)
			{
				temp.push_back(ValueType());
				se_serial(_serial, temp.back(), std::to_string(i));
			}
			std::swap(temp, _vector);
			return true;
		}
	}


	template<typename T, typename Predicate>
	void eraseIf(std::vector<T>& _vec, const Predicate& pred)
	{
		_vec.erase(std::remove_if(_vec.begin(), _vec.end(), pred), _vec.end());
	}
}

template<typename T>
T* tryFind(std::vector<T>& vector, const T& t)
{
	for (T& value : vector)
	{
		if (value == t)
		{
			return &value;
		}
	}
	return nullptr;
}
template<typename T>
const T* tryFind(const std::vector<T>& vector, const T& t)
{
	for (const T& value : vector)
	{
		if (value == t)
		{
			return &value;
		}
	}
	return nullptr;
}

template<typename T>
bool findAndErase(std::vector<T>& vector, const T& t)
{
	for (size_t i = 0; i < vector.size(); i++)
	{
		if (vector[i] == t)
		{
			vector.erase(vector.begin() + i);
			return true;
		}
	}
	return false;
}

template<typename T>
bool eraseIndex(std::vector<T>& vector, const size_t index)
{
	if (index < vector.size())
	{
		vector.erase(vector.begin() + index);
		return true;
	}
	return false;
}

template<typename T>
bool findAndSwapOut(std::vector<T>& vector, const T& t)
{
	for (size_t i = 0; i < vector.size(); i++)
	{
		if (vector[i] == t)
		{
			std::swap(vector[i], vector.back());
			vector.pop_back();
			return true;
		}
	}
	return false;
}

template<typename T>
bool swapOutIndex(std::vector<T>& vector, const size_t index)
{
	if (index < vector.size())
	{
		std::swap(vector[index], vector.back());
		vector.pop_back();
		return true;
	}
	return false;
}
