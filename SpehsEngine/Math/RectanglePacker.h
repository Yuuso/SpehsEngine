#pragma once

#include <memory>
#include <stdint.h>


namespace se
{
	struct Rectangle
	{
		Rectangle() = default;
		Rectangle(const uint16_t _width, const uint16_t _height)
			: width(_width), height(_height) {}
		Rectangle(const uint16_t _x, const uint16_t _y, const uint16_t _width, const uint16_t _height)
			: x(_x), y(_y), width(_width), height(_height) {}
		uint16_t x = 0;
		uint16_t y = 0;
		uint16_t width = 0;
		uint16_t height = 0;
	};

	// NOTE: Currently assuming that top left is origin

	class RectanglePacker
	{
	public:

		RectanglePacker(const uint16_t _width, const uint16_t _height);

		void clear();

		// Returns false if given rectangle doesn't fit
		bool addRectangle(Rectangle& _rectangle);

		uint16_t getWidth() const;
		uint16_t getHeight() const;

	private:

		struct Node
		{
			Node(const Rectangle& _rectangle);
			bool insert(Rectangle& _rectangle);

			const Rectangle rectangle;
			std::unique_ptr<Node> children[2] = { nullptr, nullptr };
			bool filled = false;
		};

		Node rootNode;
		uint16_t width = 0;
		uint16_t height = 0;
	};
}
