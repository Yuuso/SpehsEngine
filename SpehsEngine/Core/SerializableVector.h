#pragma once
#include <stdint.h>
#include <vector>
#include <limits>
#include <algorithm>
#include "SpehsEngine/Core/Serializable.h"

namespace spehs
{
	/**
	Class for reading/writing/storing vectors of data.
	Vector width type must be provided. Providing a shorter width will decrease data usage but limit the maximum element count of the vector!
	Push back/erase won't invalidate pointers or references to data vectors.
	*/
	template <class TSerializable, typename WidthType>
	class SerializableVector : public Serializable
	{
		static_assert(std::is_integral<WidthType>::value, "Serializable vector width type must be integral!");
	public:
		SerializableVector(){}
		SerializableVector(const SerializableVector<TSerializable, WidthType>& other)
		{
			*this = other;
		}
		~SerializableVector()
		{
			for (unsigned i = 0; i < data.size(); i++)
				delete data[i];
		}
		void operator=(const SerializableVector<TSerializable, WidthType>& other)
		{
			clear();
			for (unsigned i = 0; i < other.data.size(); i++)
				data.push_back(new TSerializable(*other.data[i]));
		}
		Serializable* clone() const override { return new SerializableVector<TSerializable, WidthType>(*this); }
		size_t packetSize() const override
		{
			size_t bytes = sizeof(WidthType);
			WidthType count = std::min((unsigned)std::numeric_limits<WidthType>::max(), data.size());
			for (WidthType i = 0; i < count; i++)
				bytes += data[i]->packetSize();
			return bytes;
		}
		size_t write(unsigned char* buffer) const
		{
			size_t offset = 0;
			WidthType count = std::min((unsigned)std::numeric_limits<WidthType>::max(), data.size());
			memcpy(&buffer[offset], &count, sizeof(count));		offset += sizeof(count);
			for (WidthType i = 0; i < count; i++)
				offset += data[i]->write(&buffer[offset]);
			return offset;
		}
		size_t read(const unsigned char* buffer)
		{
			clear();
			size_t offset = 0;
			WidthType size;
			memcpy(&size, &buffer[offset], sizeof(size));		offset += sizeof(size);

			if (size == 0)
				return offset;

			data.reserve(size);
			for (unsigned i = 0; i < size; i++)
			{
				data.push_back(new TSerializable());//!
				offset += data.back()->read(&buffer[offset]);
			}
			return offset;
		}

		//Managing serializable data
		void push_back(TSerializable* dataPtr)
		{
			data.push_back(dataPtr);
		}
		template <typename T>//Allows overridden == operators to be used...
		bool contains(const T& value) const
		{
			for (unsigned i = 0; i < data.size(); i++)
			{
				if (*data[i] == value)
					return true;
			}
			return false;
		}
		/// Searches for element in data vector. If element is found it is deleted and returns true.
		bool remove(TSerializable* element)
		{
			for (unsigned i = 0; i < data.size(); i++)
				if (data[i] == element)
				{
					delete element;
					data.erase(data.begin() + i);
					return true;
				}

			return false;
		}
		/// Removes element at index
		void remove(unsigned index)
		{
			if (index >= data.size())
				return;
			delete data[index];
			data.erase(data.begin() + index);
		}
		void clear()
		{
			for (unsigned i = 0; i < data.size(); i++)
				delete data[i];
			data.clear();
		}

		//accessing data
		TSerializable* operator[](int index) const
		{
			return data[index];
		}
		TSerializable* at(int index) const
		{
			return data[index];
		}
		TSerializable* back() const
		{
			return data.back();
		}
		unsigned size() const
		{
			return data.size();
		}
	private:
		std::vector<TSerializable*> data;
	};
}