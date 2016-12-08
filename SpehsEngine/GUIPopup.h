#pragma once
#include <functional>
#include "GUIRectangleContainer.h"


namespace spehs
{
	//Popup option
	struct GUIPopupOption
	{
		GUIPopupOption(std::string _string) : string(_string), hasCallback(false)
		{
		}
		GUIPopupOption(std::string _string, std::function<void()> _callback) : string(_string), callback(_callback), hasCallback(true)
		{			
		}
		~GUIPopupOption()
		{
		}
		const std::function<void()> callback;
		const std::string string;
		const bool hasCallback;
	};

	///GUI Popup
	/*
	First element in the elements vector is the message/header element. Other elements are option buttons displayed in a row under the header.
	When popup option is selected, GUIRECT_REMOVE_BIT is enabled
	*/
	class GUIPopup : public GUIRectangleContainer
	{
	public:
		template<typename ... Args>
		GUIPopup(std::string _message, GUIPopupOption& _option, Args& ... _moreOptions) : GUIPopup(_message)
		{
			addOptions(_option, _moreOptions...);
		}
		GUIPopup(std::string _message);
		~GUIPopup();

		void addOptions(GUIPopupOption& option);
		template<typename ... Args>
		void addOptions(GUIPopupOption& option, Args& ... moreOptions)
		{
			addOptions(option);
			addOptions(moreOptions...);
		}

		void inputUpdate();

		void updateMinSize();
		void updateScale();
		void updatePosition();

		void setBackgroundColor(glm::vec3& rgb);
		void setBackgroundColor(glm::vec4& rgba);
		void setBackgroundColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
		void enableEscape(){ escapeEnabled = true; }
		void disableEscape(){ escapeEnabled = false; }

	private:
		std::vector<GUIPopupOption> options;
		bool escapeEnabled;
	};
}