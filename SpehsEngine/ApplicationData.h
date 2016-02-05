#pragma once
#include <vector>
#include <string>
#include <stdint.h>


/*Purpose of this class:
-Save and manage application wide data like window size and window mode
*/
namespace SpehsEngine
{
	class ApplicationData
	{
	public:
		ApplicationData();
		~ApplicationData();

		int windowMode;// 0 windowed / 1 fullscreen
		int masterVolume;
		int musicVolume;
		int sfxVolume;
		int showFps;
		int consoleTextSize;
		int consoleTextAlpha;
		int vSync;
		int MSAA;
		std::vector<std::string> gameNames;

		//Window dimensions
		void setWindowWidth(int w){ windowWidth = w; windowWidthHalf = w / 2.0f; }
		void setWindowHeight(int h){ windowHeight = h; windowHeightHalf = h / 2.0f; }
		int getWindowWidth(){ return windowWidth; }
		int getWindowHeight(){ return windowHeight; }
		int getWindowWidthHalf(){ return windowWidthHalf; }
		int getWindowHeightHalf(){ return windowHeightHalf; }

		void save();
		void load();

	private:
		int windowWidth;
		int windowHeight;
		int windowWidthHalf;
		int windowHeightHalf;
	};
}
extern SpehsEngine::ApplicationData* applicationData;