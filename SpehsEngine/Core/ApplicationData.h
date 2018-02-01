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
	




	/*
		static manager class for reading and writing appvars.
		The appvars file is divided into sections.
		Each section consists of a group of appvars.
		Each appvar needs to have a name that is unique in its section.
	*/
	class Appvars
	{
	private:
		class Section;

		class VarBase
		{
		public:

			const std::string name;
			const std::string type;

		protected:
			friend class Appvars;
			friend class Section;
			VarBase(Section& _section, const std::string& name, const std::string& type);
			virtual ~VarBase();
			void markForUnwrittenChanges();
			bool tryRetrieveValue();

			/* Called when the appvar is written to the human readable format file. */
			virtual std::string toString() const = 0;
			/* Called when the appvar is read from the human readable format file. */
			virtual bool fromString(const std::string& string) = 0;
		private:
			Section& section;
		};

	public:
		template<typename T>
		class Var : public VarBase
		{
		public:
			T operator=(const T newValue)
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
			boost::signal<void(const Var<T>&)> changedSignal;
		private:
			friend class Section;
			Var(Section& _section, const std::string& _name, const T& _value)
				: VarBase(_section, _name, typeid(T).name())
				, value(_value)
			{
			}
			~Var() {}
			Var(const Var<T>& other) = delete;
			Var(const Var<T>&& other) = delete;
			void operator=(const Var<T>& other) = delete;
			void operator=(const Var<T>&& other) = delete;
			mutable std::recursive_mutex mutex;
			T value;
		};

	public:

		Appvars(const std::string& fileName);
		~Appvars();
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
		friend class VarBase;
		mutable std::recursive_mutex mutex;
		bool unwrittenChanges;
		
		class Section
		{
		public:
			Section(Appvars& _appvars, const std::string& _name) : appvars(_appvars), name(_name) {}
			~Section();

			template<typename T>
			Var<T>& get(const std::string& _type, const std::string& _name, const T& _defaultValue)
			{
				for (size_t i = 0; i < vars.size(); i++)
				{
					if (vars[i]->type == _type && vars[i]->name == _name)
						return (Var<T>&)(*vars[i]);
				}
				Var<T>* var = new Var<T>(*this, _name, _defaultValue);
				vars.push_back(var);
				appvars.unwrittenChanges = true;
				return *var;
			}

			const std::string name;
			Appvars& appvars;
			std::vector<VarBase*> vars;

			struct ReadVar
			{
				ReadVar(const std::string& _type, const std::string& _name, const std::string& _value) : type(_type), name(_name), value(_value) {}
				std::string type;
				std::string name;
				std::string value;
			};
			ReadVar* findReadVar(const std::string& type, const std::string& name);
			std::vector<ReadVar> readVars;
		};
		std::vector<Section*> sections;
		Section& getSection(const std::string& name);
	public:
		/* Returns a reference to a var. If var didn't exist before, it is added with the specified default value. */
		template<typename T>
		Var<T>& get(const std::string& section, const std::string& name, const T& defaultValue)
		{
			//Get/create section and var
			const std::string type = typeid(T).name();
			Var<T>& var = getSection(section).get<T>(type, name, defaultValue);
			var.tryRetrieveValue();
			return var;
		}
	};
}