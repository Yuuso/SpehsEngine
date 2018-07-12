#pragma once
namespace se
{
	class Window;
	class GLContext
	{
	public:

		bool isValid() const { return valid; }

		Window& window;

	private:
		friend class Window;
		GLContext(Window& window);
		~GLContext();
		void* glContext;
		bool valid;
	};
}