#pragma once
#include <vector>
#include <string>
#include <stdint.h>
#include <fstream>
#include <functional>
#define APPLICATION_DATA_FILENAME "ApplicationData.ini"


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
		/**Writes application data to a file*/
		static bool write();
		/**Reads application data from a file*/
		static bool read();
		/**Set a callback function to call when writing to a stream. Set before spehs engine initialization. */
		static void setWriteCallback(const std::function<void(std::ofstream&)> callbackFunction);
		/**Set a callback function to call when reading from a stream. Set before spehs engine initialization. */
		static void setReadCallback(const std::function<void(std::ifstream&)> callbackFunction);

		//Window dimensions
		static void setWindowWidth(int w);
		static void setWindowHeight(int h);
		static int getWindowWidth();
		static int getWindowHeight();
		static int getWindowWidthHalf();
		static int getWindowHeightHalf();

		//Master volume
		static void setMasterVolume(const float amount);
		static float getMasterVolume();

		//Video
		static int windowMode;// 0 windowed / 1 fullscreen
		static int showFps;
		static int maxFps;
		static int vSync;
		static int MSAA;

		//Other
		static int consoleTextSize;
		static float consoleTextAlpha;
		static int GUITextSize;
		static std::string GUITextFontPath;
		static std::string dataDirectory;//Specified directory where data should be stored. Contains the forward slash suffix
		static std::string screenshotDirectory;

	};
}