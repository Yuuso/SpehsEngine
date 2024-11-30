#pragma once

#include "SpehsEngine/Core/Murmur3.h"


namespace se
{
	namespace net
	{
		struct Address
		{
			enum class Type : uint8_t { None = 0, Ipv4 = 1, Ipv6 = 2 };

			static const Address invalid;

			struct HashFunctor
			{
				constexpr size_t operator()(const Address& address) const
				{
					switch (address.type)
					{
					case Type::None: break;
					case Type::Ipv4: return size_t(address.data0);
					case Type::Ipv6: return size_t(Murmur3::impl((const char *)&address.data0, 16, 0));
					}
					return 0;
				}
			};

			constexpr Address() = default;
			static Address makeFromStringIpv4(const std::string_view& _string);
			constexpr Address(const uint32_t& _valueIpv4) : data0(_valueIpv4), type(Type::Ipv4) {}
			constexpr Address(const uint8_t* const _bytesIpv6);

			constexpr bool operator==(const Address& other) const;
			constexpr bool operator!=(const Address& other) const;
			constexpr explicit operator bool() const { return type != Type::None; }
			constexpr inline bool isValid() const { return type != Type::None; }
			std::string toString() const;

			constexpr bool isIpv4() const { return type == Type::Ipv4; }
			constexpr bool isIpv6() const { return type == Type::Ipv6; }

			uint32_t data0 = 0;
			uint32_t data1 = 0;
			uint32_t data2 = 0;
			uint32_t data3 = 0;
			Type type = Type::None;
		};

		constexpr Address::Address(const uint8_t* const _bytesIpv6)
			: type(Type::Ipv6)
		{
			for (size_t i = 0; i < 4; i++)
			{
				*(&data0 + i) = *(const uint32_t*)_bytesIpv6[0 + i * 4];
			}
		}

		constexpr bool Address::operator==(const Address& other) const
		{
			if (type != other.type)
			{
				return false;
			}
			switch (type)
			{
			case Type::None: break;
			case Type::Ipv4: return data0 == other.data0;
			case Type::Ipv6: return data0 == other.data0 && data1 == other.data1 && data2 == other.data2 && data3 == other.data3;
			}
			return true;
		}

		constexpr bool Address::operator!=(const Address& other) const
		{
			if (type == other.type)
			{
				return false;
			}
			switch (type)
			{
			case Type::None: break;
			case Type::Ipv4: return data0 != other.data0;
			case Type::Ipv6: return data0 != other.data0 && data1 != other.data1 && data2 != other.data2 && data3 != other.data3;
			}
			return true;
		}
	}
}

template<> template<typename S, typename T>
static bool se::Serial<se::net::Address>::serial(S& _serial, T _value)
{
	se_serial(_serial, _value.data0, "data0");
	se_serial(_serial, _value.data1, "data1");
	se_serial(_serial, _value.data2, "data2");
	se_serial(_serial, _value.data3, "data3");
	se_serial(_serial, _value.type, "type");
	return true;
}

namespace std
{
	template<> struct hash<se::net::Address>
	{
		size_t operator()(const se::net::Address& p_value) const
		{
			return se::net::Address::HashFunctor()(p_value);
		}
	};
	template<> struct less<se::net::Address>
	{
		size_t operator()(const se::net::Address& a, const se::net::Address& b) const
		{
			return se::net::Address::HashFunctor()(a) < se::net::Address::HashFunctor()(b);
		}
	};
}
