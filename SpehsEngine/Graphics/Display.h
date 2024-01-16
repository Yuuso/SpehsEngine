#pragma once


namespace se::gfx
{
	struct DisplayMode
	{
		int width = 0;
		int height = 0;
		int refreshRate = 0;
	};

	struct DisplayDPI
	{
		float diagonalDPI = 0.0f;
		float horizontalDPI = 0.0f;
		float verticalDPI = 0.0f;
	};

	class Display
	{
	public:

		Display(int _index);

		int getIndex() const;
		const char* getName() const;
		glm::ivec2 getSize() const;
		DisplayDPI getDPI() const;
		std::vector<DisplayMode> getModes() const;
		int getRefreshRate() const;

	private:

		void logError(const std::string& _message) const;

		const int displayIndex = 0;
	};

	std::vector<Display> getDisplayList();
}
