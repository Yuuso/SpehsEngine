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
}