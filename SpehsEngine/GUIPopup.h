#pragma once
#include <functional>
#include "GUIRectangleContainer.h"


namespace spehs
{
	//Popup option
	struct GUIPopupOption
	{
		GUIPopupOption(){}
		GUIPopupOption(std::string _string, std::function<void()>* _callback) : string(_string), callback(nullptr)
		{
			if (_callback)
				callback = new std::function<void()>(*_callback);
		}
		~GUIPopupOption(){ if (callback) delete callback; }
		const std::string string = "";
		std::function<void()>* callback = nullptr;
	};

	///GUI Popup
	/*
	First element in the elements vector is the message/header element. Other elements are option buttons displayed in a row under the header
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

		void updatePosition();
		void updateScale();
		void updateMinSize();

	private:
		std::vector<GUIPopupOption> options;
	};
}