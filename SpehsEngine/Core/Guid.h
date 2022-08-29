#pragma once
#include <stdint.h>
#include <string>

namespace se
{
	class WriteBuffer;
	class ReadBuffer;

	/*
		Guid (Globally unique identifier) consists of the following data:
		Index: 32 bit identifier value that is relevant to the set flags and metadata.
		Flags: 16 bit flag field.
		Metadata: 16 bit data field that is relevant to the currently set flags.
	*/
	class Guid
	{
	public:

		/* Guid flags are defined on the engine level. */
		enum class Flag : uint16_t
		{
			none = 0,
			engine = 1 << 0, // the guid refers to an engine object
			temporary = 1 << 1, // the guid refers to a temporary type
			// reserved flags are reserved for future engine use
			reserved6 = 1 << 7,
			reserved5 = 1 << 6,
			reserved4 = 1 << 5,
			reserved3 = 1 << 4,
			reserved2 = 1 << 3,
			reserved1 = 1 << 2,
			// project flags are reserved for project use
			project1 = 1 << 8,
			project2 = 1 << 9,
			project3 = 1 << 10,
			project4 = 1 << 11,
			project5 = 1 << 12,
			project6 = 1 << 13,
			project7 = 1 << 14,
			project8 = 1 << 15,
		};

		struct HashFunctor
		{
			size_t operator()(const Guid& guid) const;
		};

		Guid() = default;
		Guid(const uint64_t u64) { (uint64_t&)(*this) = u64; }
		template<typename Metadata>
		Guid(const Flag _flags, const Metadata _metadata, const uint32_t _index)
			: index(_index)
			, flags(uint16_t(_flags))
			, metadata(uint16_t(_metadata))
		{
			static_assert(sizeof(Metadata) == sizeof(metadata), "Metadata type size is invalid.");
			static_assert(std::is_integral<Metadata>::value || std::is_enum<Metadata>::value, "Metadata type is invalid.");
		}

		void write(se::WriteBuffer& writeBuffer) const;
		bool read(se::ReadBuffer& readBuffer);

		bool operator==(const Guid& other) const { return toU64() == other.toU64(); }
		bool operator!=(const Guid& other) const { return toU64() != other.toU64(); }
		explicit operator bool() const { return index != invalidIndexValue; }
		uint64_t toU64() const
		{
			static_assert(sizeof(Guid) == sizeof(uint64_t), "Guid size must be 64 bit.");
			return (uint64_t&)(*this);
		}

		void setIndex(const uint32_t _index) { index = _index; }
		uint32_t getIndex() const { return index; }

		Flag getFlags() const { return Flag(flags); }
		void setFlags(const Flag _flags) { flags = uint16_t(_flags); }
		void enable(const Flag _flag) { flags = flags | uint16_t(_flag); }
		void disable(const Flag _flag) { flags = flags & ~uint16_t(_flag); }
		void toggle(const Flag _flag) { flags = flags ^ uint16_t(_flag); }
		/* Returns true if ALL given flags are enabled. */
		bool check(const Flag _flags) const { return (flags & uint16_t(_flags)) == uint16_t(_flags); }

		void setMetadata(const uint16_t _metadata) { metadata = _metadata; }
		uint16_t getMetadata() const { return metadata; }
		template<typename Metadata>
		void setMetadata(const Metadata _metadata)
		{
			static_assert(std::is_integral<Metadata>::value || std::is_enum<Metadata>::value, "Metadata type is invalid.");
			static_assert(sizeof(Metadata) == sizeof(metadata), "Metadata size is invalid.");
			metadata = uint16_t(_metadata);
		}
		template<typename Metadata>
		Metadata getMetadata() const
		{
			static_assert(std::is_integral<Metadata>::value || std::is_enum<Metadata>::value, "Metadata type is invalid.");
			static_assert(sizeof(Metadata) == sizeof(metadata), "Metadata size is invalid.");
			return Metadata(metadata);
		}

		std::string toString() const;

	private:
		static const uint32_t invalidIndexValue = ~0u;
		uint32_t index = invalidIndexValue;
		uint16_t flags = 0u;
		uint16_t metadata = 0u;
	};
}

namespace std
{
	template<> struct hash<se::Guid>
	{
		size_t operator()(const se::Guid& p_value) const
		{
			return se::Guid::HashFunctor()(p_value);
		}
	};
	template<> struct less<se::Guid>
	{
		size_t operator()(const se::Guid& a, const se::Guid& b) const
		{
			return se::Guid::HashFunctor()(a) < se::Guid::HashFunctor()(b);
		}
	};
}
