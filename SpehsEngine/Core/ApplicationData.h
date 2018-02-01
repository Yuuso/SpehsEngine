#pragma once
#include <mutex>
#include <vector>
#include <string>
#include <stdint.h>
#include <fstream>
#include <functional>
#include <boost/signal.hpp>
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
		//static void setWindowWidth(int w);
		//static void setWindowHeight(int h);
		//static int getWindowWidth();
		//static int getWindowHeight();
		//static int getWindowWidthHalf();
		//static int getWindowHeightHalf();

		//Master volume
		static void setMasterVolume(const float amount);
		static float getMasterVolume();

		//Video
		static int fullscreen;// 0 windowed / 1 fullscreen
		static int showFps;
		static int maxFps;
		static int vSync;
		static int MSAA;

		//Other
		static int consoleTextSize;
		static int consoleTextAlpha;
		static int GUITextSize;
		static std::string GUITextFontPath;
		static std::string dataDirectory;//Specified directory where data should be stored. Contains the forward slash suffix
		static std::string screenshotDirectory;
	};
	




	class AppvarBase;
	/*
		static manager class for reading and writing appvars.
		The appvars file is divided into sections.
		Each section consists of a group of appvars.
		Each appvar needs to have a name that is unique in its section.
	*/
	class Appvars
	{
	public:

		Appvars(const std::string& fileName);
		Appvars() = delete;
		Appvars(const Appvars& other) = delete;
		Appvars(const Appvars&& other) = delete;
		
		/* This is the appvars file name for the file, that resides in the working directory of the executable. */
		const std::string name;
		/* File extension, includes the dot. */
		static const std::string fileExtension;

		/* Writes to file if there are changes. */
		void update();
		/* This function can be used to query whether any appvars have changed since the last write. */
		bool hasUnwrittenChanges();
		/* Reads appvars from the appvars file. */
		bool read();
		/* Writes all currently existing appvars into the appvars file. */
		bool write();
		
	protected:
		/*
			Appvars are contained inside sections.
			Two different sections can contain an appvar with the same name.
		*/
		class Section
		{
		public:
			Appvars& appvars;
			const std::string name;
		private:
			friend class Appvars;
			friend class AppvarBase;
			struct ReadAppvar
			{
				ReadAppvar(const std::string& _name, const std::string& _value) : name(_name), value(_value) {}
				std::string name;
				std::string value;
			};

			Section(Appvars& _appvars, const std::string& _name) : appvars(_appvars), name(_name) {}
			~Section();

			/*
				Returns false if the appvar's name identifier has already been taken, and the appvar cannot be added to this section.
				If the appvar has a readable value waiting, it will be read.
			*/
			bool add(AppvarBase& appvar);
			/* Removes the appvar from the list of appvars. */
			void remove(AppvarBase& appvar);
			/* Try to find a read appvar with the specified name. */
			ReadAppvar* findReadAppvar(const std::string& name);

			std::vector<AppvarBase*> appvarBases;
			std::vector<ReadAppvar> readAppvars;
		};
		friend class AppvarBase;
		mutable std::recursive_mutex mutex;
		bool unwrittenChanges;
		/*
			NOTE: sections are dynamically allocated, and are never deallocated once created.
		*/
		std::vector<Section*> sections;
		/*
			Creates a new section with the provided name if one does not already exist.
		*/
		Section& getSection(const std::string& name);
	};
	
	/*
		Base class for creating custom appvars.
		The deriving class can decide on the set/get implementations.
		markForUnwrittenChanges() should be taken into consideration when writing a modifying method.
		An appvar must have a name and a value, both of which must be representable in a human readable string.
		The appvar is bound to the associated Appvars manager for the duration of its lifetime.
	*/
	class AppvarBase
	{
	public:
		AppvarBase(Appvars& appvars, const std::string& section, const std::string& name);
		~AppvarBase();

		/*
			Try to retrieve value from appvars' read values.
		*/
		bool tryRetrieveValue();
		/*
			Called when the appvar is written to the human readable format file.
		*/
		virtual std::string toString() const = 0;
		/*
			Called when the appvar is read from the human readable format file.
		*/
		virtual bool fromString(const std::string& string) = 0;
		
		Appvars& appvars;
		const std::string name;
		
	protected:
		/*
			The derived class can call this method to indicate the system that there are unwritten changes, and the appvars should be written.
		*/
		void markForUnwrittenChanges()
		{
			std::lock_guard<std::recursive_mutex> lock(section.appvars.mutex);
			section.appvars.unwrittenChanges = true;
		}
	private:
		friend class Appvars::Section;
		Appvars::Section& section;
	};
	
	/*
		Template class for creating generic appvars.
	*/
	template<typename T>
	class Appvar : public AppvarBase
	{
	public:
		Appvar(Appvars& _appvars, const std::string& _section, const std::string& _name, const T& defaultValue)
			: AppvarBase(_appvars, _section, _name)
			, value(defaultValue)
		{
			tryRetrieveValue();
		}

		T& operator=(const T& newValue)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (value != newValue)
			{
				value = newValue;
				markForUnwrittenChanges();
				changedSignal(*this);
			}
			return value;
		}

		operator const T&() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return value;
		}

		operator T&()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return value;
		}

		operator T() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return value;
		}

		std::string toString() const override
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return std::to_string(value);
		}

		bool fromString(const std::string& string) override
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			value = std::atoi(string.c_str());
			return true;
		}

		boost::signal<void(const Appvar<T>&)> changedSignal;

	private:
		mutable std::recursive_mutex mutex;
		T value;
	};
}