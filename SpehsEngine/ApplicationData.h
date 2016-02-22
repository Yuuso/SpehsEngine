#pragma once
#include <vector>
#include <string>
#include <stdint.h>
#include <fstream>


/*Purpose of this class:
-Save and manage application wide data like window size and window mode
*/
namespace SpehsEngine
{
	class ApplicationData
	{
	public:
		ApplicationData();
		virtual ~ApplicationData();
		///Writes application data to a txt file. If a stream pointer is given, writes to that stream instead.
		virtual void write(std::ofstream* stream = nullptr);
		///Writes application data to a txt file. If a stream pointer is given, reads from that stream instead.
		virtual void read(std::ifstream* stream = nullptr);
		/**Reads line until ':' is encountered, reads rest as int and assigns to integer. For example the line "data value: 5" would assign 5 to integer*/
		void readValueIntoInt(std::ifstream& stream, int& integer);
		/**Reads line until ':' is encountered, reads rest as string and assigns to string. Erases space after ':' if one exists*/
		void readValueIntoString(std::ifstream& stream, std::string& string);

		int windowMode;// 0 windowed / 1 fullscreen
		int masterVolume;
		int musicVolume;
		int sfxVolume;
		int showFps;
		int consoleTextSize;
		int consoleTextAlpha;
		int GUITextSize;
		std::string GUITextFontPath;
		int vSync;
		int MSAA;
		std::string dataDirectory;//Specified directory where data should be stored

		//Window dimensions
		void setWindowWidth(int w){ windowWidth = w; windowWidthHalf = w / 2; }
		void setWindowHeight(int h){ windowHeight = h; windowHeightHalf = h / 2; }
		int getWindowWidth(){ return windowWidth; }
		int getWindowHeight(){ return windowHeight; }
		int getWindowWidthHalf(){ return windowWidthHalf; }
		int getWindowHeightHalf(){ return windowHeightHalf; }

	private:
		int windowWidth;
		int windowHeight;
		int windowWidthHalf;
		int windowHeightHalf;
	};
}
extern SpehsEngine::ApplicationData* applicationData;