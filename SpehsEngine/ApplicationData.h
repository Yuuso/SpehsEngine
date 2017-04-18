#pragma once
#include <vector>
#include <string>
#include <stdint.h>
#include <fstream>


/*Purpose of this class:
-Save and manage application wide data like window size and window mode
*/
namespace spehs
{
	/**Reads line until ':' is encountered, reads rest as int and assigns to integer. For example the line "data value: 5" would assign 5 to integer*/
	void readValueIntoInt(std::ifstream& stream, int& integer);
	/**Reads line until ':' is encountered, reads rest as string and assigns to string. Erases space after ':' if one exists*/
	void readValueIntoString(std::ifstream& stream, std::string& string);

	class ApplicationData
	{
	public:
		ApplicationData();
		virtual ~ApplicationData();
		///Writes application data to a txt file. If a stream pointer is given, writes to that stream instead.
		virtual void write(std::ofstream* stream = nullptr);
		///Writes application data to a txt file. If a stream pointer is given, reads from that stream instead.
		virtual void read(std::ifstream* stream = nullptr);

		//Video
		int windowMode;// 0 windowed / 1 fullscreen
		int showFps;
		int maxFps;
		int vSync;
		int MSAA;
		//Other
		int consoleTextSize;
		int consoleTextAlpha;
		int GUITextSize;
		std::string GUITextFontPath;
		std::string dataDirectory;//Specified directory where data should be stored. Contains the forward slash suffix
		std::string screenshotDirectory;

		//Window dimensions
		void setWindowWidth(int w){ windowWidth = w; windowWidthHalf = w / 2; }
		void setWindowHeight(int h){ windowHeight = h; windowHeightHalf = h / 2; }
		int getWindowWidth() const { return windowWidth; }
		int getWindowHeight() const { return windowHeight; }
		int getWindowWidthHalf() const { return windowWidthHalf; }
		int getWindowHeightHalf() const { return windowHeightHalf; }

		//Master volume
		void setMasterVolume(const float amount);
		float getMasterVolume() const { return masterVolume; }

	private:
		int windowWidth;
		int windowHeight;
		int windowWidthHalf;
		int windowHeightHalf;
		float masterVolume;
	};
}
extern spehs::ApplicationData* applicationData;