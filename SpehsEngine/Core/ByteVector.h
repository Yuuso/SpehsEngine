#pragma once

namespace se
{
	// Strong type for a raw data block used in archives
	class ByteVector
	{
	public:
		ByteVector() = default;
		ByteVector(const ByteVector& _copy) : data(_copy.data) {}
		ByteVector(const std::vector<std::byte>& _data) : data(_data) {}
		ByteVector(ByteVector&& _move) : data(std::move(_move.data)) {}
		ByteVector(std::vector<std::byte>&& _data) : data(std::move(_data)) {}
		void operator=(const ByteVector& _copy) { data = _copy.data; }
		void operator=(const std::vector<std::byte>& _data) { data = _data; }
		void operator=(ByteVector&& _move) { data = std::move(_move.data); }
		void operator=(std::vector<std::byte>&& _data) { data = std::move(_data); }
		inline const std::byte* getData() const { return data.data(); }
		inline size_t getSize() const { return data.size(); }
		void swap(std::vector<std::byte>& _data) { data.swap(_data); }
		void clear() { data.clear(); }
	private:
		std::vector<std::byte> data;
	};
}