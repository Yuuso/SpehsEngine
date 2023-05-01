#pragma once

namespace se
{
	// Memory is not owned
	class ByteView
	{
	public:
		ByteView(const std::byte* const _data, const size_t _size)
			: data(_data)
			, size(_size)
		{
		}
		inline const std::byte* getData() const { return data; }
		inline size_t getSize() const { return size; }
	private:
		const std::byte* data = nullptr;
		size_t size = 0;
	};

	template<size_t Length>
	class StaticByteView
	{
	public:
		StaticByteView(std::byte* const _data) : data(_data) {}
		inline std::byte* getData() { return data; }
		inline const std::byte* getData() const { return data; }
		static constexpr size_t getSize() { return Length; }
	private:
		std::byte* data = nullptr;
	};

	template<size_t Length>
	class ConstStaticByteView
	{
	public:
		ConstStaticByteView(const std::byte* const _data) : data(_data) {}
		inline const std::byte* getData() const { return data; }
		static constexpr size_t getSize() { return Length; }
	private:
		const std::byte* data = nullptr;
	};
}