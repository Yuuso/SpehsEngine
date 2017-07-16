#pragma once
#include <functional>
#include "GUIRectangleContainer.h"


namespace spehs
{
	///GUI Popup
	/*
	First element in the elements vector is the message/header element. Other elements are option buttons displayed in a row under the header.
	When popup option is selected, GUIRECT_REMOVE_BIT is enabled
	*/
	class GUIPopup : public GUIRectangleContainer
	{
	public:
		//Button popup option
		struct Option
		{
			//Textfield constructor
			Option() : string(), hasPressCallback(false)
			{
			}
			//Cancel constructor
			Option(const std::string& _string) : string(_string), hasPressCallback(false)
			{
			}
			//Press callback constructor
			Option(const std::string& _string, std::function<void()> _callback) : string(_string), callback(_callback), hasPressCallback(true)
			{
			}
			~Option()
			{
			}
			const std::string string;
			const std::function<void()> callback;
			const bool hasPressCallback;
		};
	public:
		template<typename ... Args>
		GUIPopup(const std::string& _message, const Option& _option, const Args& ... _moreOptions) : GUIPopup(_message)
		{
			addOptions(_option, _moreOptions...);
		}
		GUIPopup(const std::string& _message);
		~GUIPopup();

		void addOptions(const Option& option);
		template<typename ... Args>
		void addOptions(const Option& option, const Args& ... moreOptions)
		{
			addOptions(option);
			addOptions(moreOptions...);
		}

		void inputUpdate();

		void updateMinSize();
		void updateScale();
		void updatePosition();

		void setBackgroundColor(const spehs::Color& rgb);
		void enableEscape(){ escapeEnabled = true; }
		void disableEscape(){ escapeEnabled = false; }

	private:
		std::vector<Option> options;
		bool escapeEnabled;
	};
}