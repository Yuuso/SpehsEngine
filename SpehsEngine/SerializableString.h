#pragma once
#include <string>
#include "Serializable.h"

namespace spehs
{
	class SerializableString : public Serializable, public std::string
	{
	public:
		SerializableString();
		SerializableString(const std::string& other);
		Serializable* clone() const { return new SerializableString(*this); }
		size_t packetSize() const override;
		size_t write(unsigned char* buffer) const override;
		size_t read(const unsigned char* buffer) override;
		std::string toString() const override { return std::string(this->begin(), this->end()); }
	};
}