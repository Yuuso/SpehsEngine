#pragma once



namespace se::gui
{
	class Element;

	class ITriggerAction
	{
	public:
		virtual void execute(Element*) const = 0;
	};
}
