#pragma once
namespace spehs
{
	class Window;
	class GLContext
	{
	public:
		GLContext(Window& window);
		~GLContext();

		bool isValid() const { return valid; }

		Window& window;

	private:
		void* glContext;
		bool valid;
	};
}