#pragma once
#include <functional>
#include "GUIRectangleContainer.h"


namespace spehs
{
	//Popup option
	struct GUIPopupOption
	{
		GUIPopupOption(const std::string& _string) : string(_string), hasCallback(false)
		{
		}
		GUIPopupOption(const std::string& _string, std::function<void()> _callback) : string(_string), callback(_callback), hasCallback(true)
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
		GUIPopup(const std::string& _message, const GUIPopupOption& _option, const Args& ... _moreOptions) : GUIPopup(_message)
		{
			addOptions(_option, _moreOptions...);
		}
		GUIPopup(const std::string& _message);
		~GUIPopup();

		void addOptions(const GUIPopupOption& option);
		template<typename ... Args>
		void addOptions(const GUIPopupOption& option, const Args& ... moreOptions)
		{
			addOptions(option);
			addOptions(moreOptions...);
		}

		void inputUpdate();

		void updateMinSize();
		void updateScale();
		void updatePosition();

		void setBackgroundColor(const glm::vec3& rgb);
		void setBackgroundColor(const glm::vec4& rgba);
		void setBackgroundColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a = 255);
		void enableEscape(){ escapeEnabled = true; }
		void disableEscape(){ escapeEnabled = false; }

	private:
		std::vector<GUIPopupOption> options;
		bool escapeEnabled;
	};
}